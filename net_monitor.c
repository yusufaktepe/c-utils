#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>

#define CHECK_INTERVAL 5      // Check every 5 seconds
#define RETRY_COUNT 4         // Number of retries before considering it down
#define NOTIFY_DOWN "notify-send -a system -i network-wireless-disconnected -r 2003 'net_monitor' 'Internet connection is down!'"
#define NOTIFY_UP "notify-send -a system -i network-wireless-connected -r 2003 'net_monitor' 'Internet connection restored'"
#define DNS_SERVER "8.8.8.8"
#define DNS_PORT 53

// Simple DNS query packet (asking for google.com)
unsigned char dns_query[] = {
    0xAA, 0xAA,  // ID
    0x01, 0x00,  // Standard query
    0x00, 0x01,  // 1 question
    0x00, 0x00,  // 0 answers
    0x00, 0x00,  // 0 authority
    0x00, 0x00,  // 0 additional
    0x06, 'g', 'o', 'o', 'g', 'l', 'e',
    0x03, 'c', 'o', 'm', 0x00,  // google.com
    0x00, 0x01,  // Type A
    0x00, 0x01   // Class IN
};

int verbose = 0;

int check_internet() {
    for (int i = 0; i < RETRY_COUNT; i++) {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) return 0;

        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(DNS_PORT);
        inet_pton(AF_INET, DNS_SERVER, &server.sin_addr);

        // Set a timeout for receiving data
        struct timeval timeout = {2, 0};  // 2 seconds
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        // Send a real DNS query
        int sent = sendto(sock, dns_query, sizeof(dns_query), 0, (struct sockaddr*)&server, sizeof(server));
        if (sent < 0) {
            close(sock);
            continue;  // Retry if sending fails
        }

        char buffer[512];
        socklen_t len = sizeof(server);
        int received = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server, &len);

        close(sock);
        if (received > 0) return 1;  // If we received a response, the internet is up

        sleep(1);  // Small delay before retrying
    }
    return 0;  // If all retries fail, internet is down
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
        verbose = 1;
    }

    int was_connected = check_internet();
    if (verbose) printf("Initial internet status: %s\n", was_connected ? "Connected" : "Disconnected");

    while (1) {
        int is_connected = check_internet();

        if (is_connected && !was_connected) {
            if (verbose) printf("Internet restored!\n");
            system(NOTIFY_UP);
        } else if (!is_connected && was_connected) {
            if (verbose) printf("Internet down!\n");
            system(NOTIFY_DOWN);
        }

        was_connected = is_connected;
        sleep(CHECK_INTERVAL);
    }

    return 0;
}

