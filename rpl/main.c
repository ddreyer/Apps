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
    int control_packets_tx;
    int dis_tx;
    int dio_tx;
    int dao_tx;
    int control_packets_rx;
    int dis_rx;
    int dio_rx;
    int dao_rx;
    int bufsize = 48;
    uint8_t buf[bufsize];
    for (int i = 0; i < 48; i++) {
        buf[i] = 0x0;
    }
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

      buf[17] = dis_rx & 0xff;
      buf[16] = (dis_rx >> 8) & 0xff; 
      buf[15] = (dis_rx >> 16) & 0xff; 
      buf[14] = (dis_rx >> 24) & 0xff;
      buf[21] = dio_rx & 0xff;
      buf[20] = (dio_rx >> 8) & 0xff; 
      buf[19] = (dio_rx >> 16) & 0xff; 
      buf[18] = (dio_rx >> 24) & 0xff;
      buf[25] = dao_rx & 0xff;
      buf[24] = (dao_rx >> 8) & 0xff; 
      buf[23] = (dao_rx >> 16) & 0xff; 
      buf[22] = (dao_rx >> 24) & 0xff;


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
      buf[5] = parent_change_cntr & 0xff;
      buf[4] = (parent_change_cntr >> 8) & 0xff; 
      buf[3] = (parent_change_cntr >> 16) & 0xff; 
      buf[2] = (parent_change_cntr >> 24) & 0xff;


      // network layer stats
      netstats_t *stats = gnrc_ipv6_netif_get_stats(7);
      printf("ipv6 tx unicast: %u\n", (unsigned int) stats->tx_unicast_count);
      printf("ipv6 tx success: %u\n", (unsigned int) stats->tx_success);
      printf("ipv6 tx failed: %u\n", (unsigned int) stats->tx_failed);
      buf[9] = stats->tx_success & 0xff;
      buf[8] = (stats->tx_success >> 8) & 0xff; 
      buf[7] = (stats->tx_success >> 16) & 0xff; 
      buf[6] = (stats->tx_success >> 24) & 0xff;
      buf[13] = stats->tx_failed & 0xff;
      buf[12] = (stats->tx_failed >> 8) & 0xff; 
      buf[11] = (stats->tx_failed >> 16) & 0xff; 
      buf[10] = (stats->tx_failed >> 24) & 0xff;
      printf("\n\n\n");

      // Tx Sequence number setting
      buf[1]++;
      if (buf[1] == 0) {
          buf[0]++;
      }

      // TODO: change this IP address to border router/root node
		  send_udp("fe80::d212:d55a:7b4e:6479",4747,buf,bufsize);
      //Sleep
		  xtimer_usleep(interval_with_jitter());
    }

    return 0;
}
