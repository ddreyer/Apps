#include <stdio.h>
#include <rtt_stdio.h>
#include "xtimer.h"
#include <string.h>
#include "net/gnrc.h"
#include "net/gnrc/rpl.h"
// #include "net/gnrc/ipv6/netif.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#ifndef SAMPLE_INTERVAL
#define SAMPLE_INTERVAL (1000000UL)
#endif
#define SAMPLE_JITTER   ( 200000UL)

#define TYPE_FIELD 8


void send_udp(char *addr_str, uint16_t port, uint8_t *data, uint16_t datalen);


void critical_error(void) {
    DEBUG("CRITICAL ERROR, REBOOT\n");
    NVIC_SystemReset();
    return;
}

uint32_t interval_with_jitter(void)
{
    int32_t t = SAMPLE_INTERVAL;
    t += rand() % SAMPLE_JITTER;
    t -= (SAMPLE_JITTER >> 1);
    return (uint32_t)t;
}

int main(void) {
    uint8_t buf[4];
    int control_packets_tx;
    int dis_tx;
    int dio_tx;
    int dao_tx;
    int control_packets_rx;
    int dis_rx;
    int dio_rx;
    int dao_rx;
    while (1) {
      //Send;
      printf("\n\n\n");
      printf("MAIN APPLICATION\n");
      // control packet counts
      control_packets_tx = gnrc_rpl_netstats.dio_tx_ucast_count
      + gnrc_rpl_netstats.dio_tx_mcast_count
      + gnrc_rpl_netstats.dis_tx_ucast_count
      + gnrc_rpl_netstats.dis_tx_mcast_count
      + gnrc_rpl_netstats.dao_tx_ucast_count
      + gnrc_rpl_netstats.dao_tx_mcast_count
      + gnrc_rpl_netstats.dao_ack_tx_ucast_count
      + gnrc_rpl_netstats.dao_ack_tx_mcast_count;
      dis_tx = gnrc_rpl_netstats.dis_tx_ucast_count
        + gnrc_rpl_netstats.dis_tx_mcast_count;
      dio_tx = gnrc_rpl_netstats.dio_tx_ucast_count
      + gnrc_rpl_netstats.dio_tx_mcast_count;
      dao_tx = gnrc_rpl_netstats.dao_tx_ucast_count
      + gnrc_rpl_netstats.dao_tx_mcast_count
      + gnrc_rpl_netstats.dao_ack_tx_ucast_count
      + gnrc_rpl_netstats.dao_ack_tx_mcast_count;
      printf("control packets tx: %d\n", control_packets_tx);
      printf("dodag solicit packets tx: %d\n", dis_tx);
      printf("dodag info packets tx: %d\n", dio_tx);
      printf("destination advertisement packets tx: %d\n", dao_tx);


      control_packets_rx = gnrc_rpl_netstats.dio_rx_ucast_count
      + gnrc_rpl_netstats.dio_rx_mcast_count
      + gnrc_rpl_netstats.dis_rx_ucast_count
      + gnrc_rpl_netstats.dis_rx_mcast_count
      + gnrc_rpl_netstats.dao_rx_ucast_count
      + gnrc_rpl_netstats.dao_rx_mcast_count
      + gnrc_rpl_netstats.dao_ack_rx_ucast_count
      + gnrc_rpl_netstats.dao_ack_rx_mcast_count;
      printf("control packets rx: %d\n", control_packets_rx);
      dis_rx = gnrc_rpl_netstats.dis_rx_ucast_count
        + gnrc_rpl_netstats.dis_rx_mcast_count;
      dio_rx = gnrc_rpl_netstats.dio_rx_ucast_count
      + gnrc_rpl_netstats.dio_rx_mcast_count;
      dao_rx = gnrc_rpl_netstats.dao_rx_ucast_count
      + gnrc_rpl_netstats.dao_rx_mcast_count
      + gnrc_rpl_netstats.dao_ack_rx_ucast_count
      + gnrc_rpl_netstats.dao_ack_rx_mcast_count;
      printf("control packets rx: %d\n", control_packets_rx);
      printf("dodag solicit packets rx: %d\n", dis_rx);
      printf("dodag info packets rx: %d\n", dio_rx);
      printf("destination advertisement packets rx: %d\n", dao_rx);

      // parent/next hop stats
      for (int i = 0; i < GNRC_RPL_PARENTS_NUMOF; i++) {
        if (gnrc_rpl_parents[i].state == 1) {
          printf("parent link local ipv6\trank\tlink metric\nlink metric type\n");
          ipv6_addr_print(&gnrc_rpl_parents[i].addr);
          printf("%u\t", gnrc_rpl_parents[i].rank);
          printf("%f\t", gnrc_rpl_parents[i].link_metric);
          printf("%u\t", gnrc_rpl_parents[i].link_metric_type);
        }
      }
      printf("number of parent changes: %d\n", parent_change_cntr);


      // network layer stats
      netstats_t *stats = gnrc_ipv6_netif_get_stats(7);
      // stats->tx_mcast_count;
      printf("ipv6 tx unicast: %u\n", (unsigned int) stats->tx_unicast_count);
      printf("ipv6 tx success: %u\n", (unsigned int) stats->tx_success);
      printf("ipv6 tx failed: %u\n", (unsigned int) stats->tx_failed);

      printf("\n\n\n");


		  send_udp("fe80::d212:d55a:7b4e:6479",4747,buf,sizeof(buf));
      //Sleep
		  xtimer_usleep(interval_with_jitter());
    }

    return 0;
}