#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "datalink.h"
#include "protocol.h"

#define DATA_TIMER 2000
#define ACK_TIMER 256
#define MAX_SEQ 0xf

typedef unsigned char seq_nr;

struct FRAME {
    unsigned char kind; /* FRAME_DATA */
    unsigned char ack;
    unsigned char seq;
    unsigned char data[PKT_LEN];
    unsigned int padding;
};

static void put_frame(unsigned char* frame, int len)
{
    *(unsigned int*)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
}

static void send_data_frame(unsigned char frame_nr, unsigned char frame_expected,
    unsigned char* packet, size_t length)
{
    struct FRAME s;

    s.kind = FRAME_DATA;
    s.seq = frame_nr;
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
    if (length > PKT_LEN) {
        dbg_frame("Error while sending packet %d with ack %d: Length too large.\n",
            s.seq, s.ack, length);
        return;
    }
    memcpy(s.data, packet, PKT_LEN);

    dbg_frame("Packet sent: seq = %d, ack = %d, data id = %d\n", s.seq, s.ack, *(short*)s.data);

    // kind + ack + seq + original data
    put_frame((unsigned char*)&s, 3 + PKT_LEN);
}

#define send_marumaru_frame(FRAME_TYPE, FRAME_TYPENAME)                 \
    static void send_##FRAME_TYPE##_frame(unsigned char frame_expected) \
    {                                                                   \
        struct FRAME s;                                                 \
                                                                        \
        s.kind = FRAME_##FRAME_TYPE;                                    \
        s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);             \
                                                                        \
        dbg_frame("Send " FRAME_TYPENAME " %d\n", s.ack);               \
                                                                        \
        put_frame((unsigned char*)&s, 2);                               \
    }

send_marumaru_frame(ACK, "ACK")

    send_marumaru_frame(NAK, "NAK")

        static inline bool bewteen(unsigned char a, unsigned char b, unsigned char c)
{
    return ((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
}

#define inc(num) num = ((num + 1) & MAX_SEQ)

// Now the main is the go_back_n version
int main(int argc, char** argv)
{
    int event, arg;
    struct FRAME f;

    seq_nr next_frame_to_send = 0;
    seq_nr ack_expected = 0;
    seq_nr frame_expected = 0;

    bool no_nak = true;
    bool phl_ready = false;

    int frame_length = 0;

    unsigned char buffer[MAX_SEQ + 1][PKT_LEN];
    int packet_length[MAX_SEQ + 1];

    seq_nr nbuffered = 0;

    protocol_init(argc, argv);
    disable_network_layer();

    for (;;) {
        event = wait_for_event(&arg);

        switch (event) {
        case NETWORK_LAYER_READY:
            // from_network_layer
            packet_length[next_frame_to_send] = get_packet(buffer[next_frame_to_send]);
			// expand the sender's window
            nbuffered++;
            // send_data
            send_data_frame(next_frame_to_send, frame_expected, buffer[next_frame_to_send], packet_length[next_frame_to_send]);
			// 
            start_timer(next_frame_to_send, DATA_TIMER);
            stop_ack_timer();

            inc(next_frame_to_send);
            phl_ready = false;
            break;
        case PHYSICAL_LAYER_READY:
            phl_ready = true;
            break;

        case FRAME_RECEIVED:
            // from_physical_layer
            frame_length = recv_frame((unsigned char*)(&f), sizeof(f));
            // bad_frame, should return nak
            if (frame_length < 5 || crc32((unsigned char*)&f, frame_length) != 0) {
                dbg_event("**** Receiver Error, Bad CRC Checksum\n");
                if (no_nak) {
                    send_NAK_frame(frame_expected);
                    no_nak = true;
                    stop_ack_timer();
                }

                break;
            }
            if (f.kind == FRAME_ACK)
                dbg_frame("Recv ACK  %d\n", f.ack);
            if (f.kind == FRAME_NAK)
                dbg_frame("Recv NAK  %d\n", f.ack);
            if (f.kind == FRAME_DATA) {
                dbg_frame("Recv DATA %d %d, ID %d\n", f.seq, f.ack, *(short*)f.data);
                if (f.seq == frame_expected) {
                    put_packet(f.data, frame_length - 7);
                    no_nak = true;
                    inc(frame_expected);
                    start_ack_timer(ACK_TIMER);
                } else if (no_nak) {
                    send_NAK_frame(frame_expected);
                    no_nak = false;
                    stop_ack_timer();
                }
            }

			// stop all the timers before
            while (bewteen(ack_expected, f.ack, next_frame_to_send)) {
                nbuffered--;
                stop_timer(ack_expected);
                inc(ack_expected);
            }

            if (f.kind == FRAME_NAK) {
                stop_timer(ack_expected + 1);
                next_frame_to_send = ack_expected;
				// resend those expected packet
                for (seq_nr i = 0; i < nbuffered; i++) {
                    send_data_frame(next_frame_to_send, frame_expected, buffer[next_frame_to_send], packet_length[next_frame_to_send]);
                    start_timer(next_frame_to_send, DATA_TIMER);
                    stop_ack_timer();

                    inc(next_frame_to_send);
                }

                phl_ready = false;
            }
            break;
        case DATA_TIMEOUT:
            dbg_event("---- DATA %d timeout\n", arg);
            next_frame_to_send = ack_expected;
            for (seq_nr i = 0; i < nbuffered; i++) {
                send_data_frame(next_frame_to_send, frame_expected, buffer[next_frame_to_send], packet_length[next_frame_to_send]);
                start_timer(next_frame_to_send, DATA_TIMER);
                stop_ack_timer();

                inc(next_frame_to_send);
            }

            phl_ready = false;
            break;
        case ACK_TIMEOUT:
            send_ACK_frame(frame_expected);
            stop_ack_timer();
            break;
        }

        if (nbuffered < MAX_SEQ && phl_ready)
            enable_network_layer();
        else
            disable_network_layer();
    }

    return 0;
}
