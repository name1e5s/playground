#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "datalink.h"
#include "protocol.h"

#define MAX_SEQ 0x1f
#define NR_BUFS ((MAX_SEQ + 1) / 2)
#define MAX_PKT 256
#define ACK_TIMER 666

#define inc(k) (k = (k + 1) & MAX_SEQ)

typedef unsigned char seq_nr; // frame nubmber
typedef unsigned char frame_type; // frame type

struct FRAME {
    unsigned char kind; /* FRAME_DATA */
    unsigned char ack;
    unsigned char seq;
    unsigned char data[MAX_PKT];
    unsigned int padding;
};

struct buffer {
    unsigned char buf[MAX_PKT];
    size_t length;
};

bool no_nak = true;
bool phl_ready = false;
bool low_error = false;
int bits_received;
int error_received;
int DATA_TIMER = 5000;

static bool between(seq_nr a, seq_nr b, seq_nr c)
{
    return ((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
}

static void put_frame_crc(unsigned char* frame, int len)
{
    *(unsigned int*)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
    phl_ready = false;
}

static void put_frame_naive(unsigned char* frame)
{
    send_frame(frame, 3);
}


static void send_link_frame(frame_type fk, seq_nr frame_nr, seq_nr frame_expceted, struct buffer buffer[])
{
    struct FRAME s;
    s.kind = fk;
    if (fk == FRAME_DATA) {
        memcpy(s.data, buffer[frame_nr % NR_BUFS].buf, buffer[frame_nr % NR_BUFS].length);
        s.seq = frame_nr;
        dbg_frame("Send DATA %d %d, ID %d\n", frame_nr,
            ((frame_expceted + MAX_SEQ) % (MAX_SEQ + 1)), *(short*)s.data);
    }
    s.ack = ((frame_expceted + MAX_SEQ) % (MAX_SEQ + 1));
    if (fk == FRAME_NAK) {
        dbg_frame("Send NAK %d\n", frame_expceted);
        no_nak = false;
    }
    if (fk == FRAME_ACK || fk == FRAME_NAK) {
            put_frame_naive((unsigned char*)&s);

    } else
        put_frame_crc((unsigned char*)&s, 3 + buffer[frame_nr % NR_BUFS].length);
    if (fk == FRAME_DATA) {
        start_timer(frame_nr % NR_BUFS, DATA_TIMER);
    }
    stop_ack_timer();
}

int main(int argc, char** argv)
{
    seq_nr ack_expected = 0;
    seq_nr next_frame_to_send = 0;
    seq_nr frame_expected = 0;
    seq_nr nbuffered = 0;
    seq_nr r_seq = 0, r_ack = 0;
    seq_nr too_far = NR_BUFS;
    int i;
    int event, arg, len;
    struct FRAME r;
    struct buffer out_buf[NR_BUFS] = {0};
    struct buffer in_buf[NR_BUFS] = {0};
    bool arrived[NR_BUFS];
    for (i = 0; i < NR_BUFS; i++)
        arrived[i] = false;
    protocol_init(argc, argv);
    disable_network_layer();
    while (true) {

        event = wait_for_event(&arg);

        switch (event) {
        case NETWORK_LAYER_READY:
            nbuffered++;
            out_buf[next_frame_to_send % NR_BUFS].length = get_packet(out_buf[next_frame_to_send % NR_BUFS].buf);
            send_link_frame(FRAME_DATA, next_frame_to_send, frame_expected, out_buf);
            inc(next_frame_to_send);
            break;

        case PHYSICAL_LAYER_READY:
            phl_ready = true;
            break;

        case FRAME_RECEIVED:
            len = recv_frame((unsigned char*)&r, sizeof r);
            bits_received += len * 8;
            if ((len < 5 && len != 3) || (len > 5 && crc32((unsigned char*)&r, len) != 0)) {
                dbg_event("****RECEIVER ERROR, BAD CRC CHECKSUM****\n");
                error_received += 1;
                if (no_nak) {
                    send_link_frame((frame_type)FRAME_NAK, 0, frame_expected, out_buf);
                }
                break;
            }

            if (r.kind == FRAME_ACK) {
                r_ack = r.ack;
                dbg_frame("Recv ACK  %d\n", r_ack);
            }

            if (r.kind == FRAME_DATA) {
                r_seq = r.seq;
                r_ack = r.ack;
                dbg_frame("Recv DATA %d %d, ID %d\n", r_seq, r_ack, *(short*)r.data);
                if ((r_seq != frame_expected) && no_nak)
                    send_link_frame((frame_type)FRAME_NAK, 0, frame_expected, out_buf);
                else
                    start_ack_timer(ACK_TIMER);
                if (between(frame_expected, r_seq, too_far) && (arrived[r_seq % NR_BUFS] == false)) {
                    arrived[r_seq % NR_BUFS] = true;
                    memcpy(in_buf[r_seq % NR_BUFS].buf, r.data, len - 7);
                    in_buf[r_seq % NR_BUFS].length = len - 7;
                    while (arrived[frame_expected % NR_BUFS]) {
                        put_packet(in_buf[frame_expected % NR_BUFS].buf, in_buf[frame_expected % NR_BUFS].length);
                        no_nak = true;
                        arrived[frame_expected % NR_BUFS] = false;
                        inc(frame_expected);
                        inc(too_far);
                    }
                }
            }

            if (r.kind == FRAME_NAK) {
                r_ack = r.ack;
                dbg_frame("Recv NAK %d\n", r_ack);
                if (between(ack_expected, (r_ack + 1) % (MAX_SEQ + 1), next_frame_to_send)) {
                    send_link_frame((frame_type)FRAME_DATA, (r_ack + 1) % (MAX_SEQ + 1), frame_expected, out_buf);
                }
            }

            while (between(ack_expected, r_ack, next_frame_to_send)) {
                nbuffered--;
                stop_timer(ack_expected % NR_BUFS);
                inc(ack_expected);
            }
            break;

        case ACK_TIMEOUT:
            dbg_event("----  ACK %d timeout\n", frame_expected);
            send_link_frame((frame_type)FRAME_ACK, 0, frame_expected, out_buf);
            break;

        case DATA_TIMEOUT:
            dbg_event("---- DATA %d timeout\n", arg);
            if (!between(ack_expected, arg, next_frame_to_send))
                arg = arg + NR_BUFS;
            send_link_frame((frame_type)FRAME_DATA, arg, frame_expected, out_buf);
            break;

        default:
            break;
        }
        if (nbuffered < NR_BUFS && phl_ready == true)
            enable_network_layer();
        else
            disable_network_layer();

        if ((error_received << 22) < bits_received) {
            low_error = true;
        } else {
            low_error = false;
        }

        if (bits_received > 0x3f3f3f3f) {
            bits_received >>= 20;
            error_received >>= 20;
        }

        if (low_error) {
            DATA_TIMER = 3500;
        } else {
            DATA_TIMER = 5000;
        }
    }
}