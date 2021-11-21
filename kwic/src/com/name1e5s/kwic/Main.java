package com.name1e5s.kwic;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;

public class Main {
    public static void main(String[] args) {
        if (Arrays.stream(args).anyMatch(s -> s.startsWith("tcp"))) {
            System.out.println("Running on TCP mode, listening on port 1145");
            try {
                ServerSocket serverSocket = new ServerSocket(1145);
                while (true) {
                    Socket socket = serverSocket.accept();
                    new Thread(() -> {
                        try {
                            BufferedReader reader = new BufferedReader(new InputStreamReader(new DataInputStream(socket.getInputStream())));
                            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(new DataOutputStream(socket.getOutputStream())));
                            run(reader, writer);
                            socket.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }).start();
                }
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(1);
            }
        } else {
            BufferedReader reader = null;
            BufferedWriter writer = null;

            if (args.length == 2) {
                System.out.println("Running on FILE IO mode");
                System.out.printf("Input file path: %s\n", args[0]);
                System.out.printf("Output file path: %s\n", args[1]);

                try {
                    reader = new BufferedReader(new InputStreamReader(new FileInputStream(args[0])));
                    writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(args[1])));
                } catch (Exception e) {
                    System.out.println("Open file failed!");
                    e.printStackTrace();
                }
            } else {
                System.out.println("Running on STDIO mode");
                reader = new BufferedReader(new InputStreamReader(System.in));
                writer = new BufferedWriter(new OutputStreamWriter(System.out));
            }
            run(reader, writer);
            System.out.println("Done.");
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
