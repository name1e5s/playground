package com.name1e5s.kwic;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;

public class Main {

    public static void main(String[] args) {
        if (Arrays.stream(args).anyMatch(s -> s.contains("tcp"))) {
            System.out.println("Running on TCP mode, listening on port 1145");
            try {
                ServerSocket serverSocket = new ServerSocket(1145);
                while (true) {
                    Socket socket = serverSocket.accept();
                    BufferedReader reader = new BufferedReader(new InputStreamReader(new DataInputStream(socket.getInputStream())));
                    BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(new DataOutputStream(socket.getOutputStream())));
                    run(reader, writer);
                    socket.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(1);
            }
        } else {
            System.out.println("Running on STDIO mode");
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(System.out));
            run(reader, writer);
        }
    }

    public static void run(BufferedReader reader, BufferedWriter writer) {
        new Input(reader)
                .andThen(new CircularShift())
                .andThen(new Alphabetizer())
                .andThen(new Output(writer))
                .iter()
                .next();
    }
}
