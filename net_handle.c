/**
 * @file      : net_handle.c
 * @brief     : Linux平台网络处理函数接口源文件
 * @author    : huenrong (huenrong1028@outlook.com)
 * @date      : 2023-08-19 12:01:25
 *
 * @copyright : Copyright (c) 2023 huenrong
 *
 * @history   : date        author          description
 *              2023-08-19  huenrong        创建文件
 *
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "net_handle.h"

/**                                                                                                                                                                                        * @brief  计算ICMP头部校验和
 * @param  src_data    : 输入参数, 待校验源数据
 * @param  src_data_len: 输入参数, 待校验源数据长度
 * @return 校验值
 */
static uint16_t icmp_check_sum(const uint16_t *src_data, const uint16_t src_data_len)
{
    uint16_t src_data_len_tmp = src_data_len;
    uint32_t sum = 0;

    while (src_data_len_tmp > 1)
    {
        sum += *src_data++;
        src_data_len_tmp -= 2;
    }

    if (src_data_len_tmp == 1)
    {
        sum += *(uint8_t *)src_data;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (uint16_t)~sum;
}

/**
 * @brief  ping指定地址
 * @param  fd       : 输入参数, 文件描述符
 * @param  ping_addr: 输入参数, 待ping的地址
 * @param  seq_num  : 输入参数, ping的序列号
 * @return 校验值
 */
static bool ping(const int fd, const struct sockaddr *ping_addr, const uint16_t seq_num)
{
#define ICMP_DEFAULT_DATA_LEN 56
#define ICMP_CUSTOM_DATA_LEN  8

    // 8字节头部 + 56字节默认数据 + 8字节自定义数据
    char send_icmp_packet[ICMP_MINLEN + ICMP_DEFAULT_DATA_LEN + ICMP_CUSTOM_DATA_LEN] = {0};
    struct icmp *send_icmp = (struct icmp *)send_icmp_packet;

    // ICMP头部
    memset(send_icmp_packet, 0, sizeof(send_icmp_packet));
    send_icmp->icmp_type = ICMP_ECHO;
    send_icmp->icmp_code = 0;
    send_icmp->icmp_id = getpid();
    send_icmp->icmp_seq = seq_num;

    // 8字节自定义数据
    memcpy(send_icmp->icmp_data, "huenrong", 8);

    // 校验和
    send_icmp->icmp_cksum = icmp_check_sum((uint16_t *)send_icmp_packet, sizeof(send_icmp_packet));

    // 发送请求
    int ret = sendto(fd, send_icmp_packet, sizeof(send_icmp_packet), 0, ping_addr, sizeof(struct sockaddr));
    if (ret <= 0)
    {
        return false;
    }

    fd_set recv_fds;
    FD_ZERO(&recv_fds);
    FD_SET(fd, &recv_fds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    ret = select(fd + 1, &recv_fds, NULL, NULL, &timeout);
    if (ret <= 0)
    {
        return false;
    }

    if (!FD_ISSET(fd, &recv_fds))
    {
        return false;
    }

    char recv_data[1024] = {0};
    socklen_t len = sizeof(*ping_addr);
    ret = recvfrom(fd, recv_data, sizeof(recv_data), 0, (struct sockaddr *)ping_addr, &len);
    if (ret < 0)
    {
        return false;
    }

    // 解析ICMP响应
    struct ip *ip_header = (struct ip *)recv_data;
    uint32_t ip_header_len = ip_header->ip_hl << 2;
    struct icmp *recv_icmp = (struct icmp *)(recv_data + ip_header_len);
    uint32_t recv_icmp_packet_len = ret - ip_header_len;

    // 校验ICMP类型和代码
    if ((recv_icmp_packet_len != sizeof(send_icmp_packet)) || (recv_icmp->icmp_type != ICMP_ECHOREPLY)
        || (recv_icmp->icmp_id != send_icmp->icmp_id) || (recv_icmp->icmp_seq != send_icmp->icmp_seq)
        || (memcmp(recv_icmp->icmp_data, send_icmp->icmp_data, ICMP_CUSTOM_DATA_LEN) != 0))
    {
        return false;
    }

    return true;
}

/**
 * @brief  获取IP地址
 * @param  ip_addr       : 输出参数, IP地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_ip_addr(char *ip_addr, const char *interface_name)
{
    assert(NULL != ip_addr);
    assert(NULL != interface_name);

    int fd = -1;
    struct ifreq ifr = {0};

    // 打开一个socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        return false;
    }

    // 设置要查询的网络接口名
    strcpy(ifr.ifr_name, interface_name);

    // 查询IP地址
    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
    {
        close(fd);

        return false;
    }
    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
    inet_ntop(AF_INET, &addr->sin_addr, ip_addr, INET_ADDRSTRLEN);

    // 关闭socket
    close(fd);

    return true;
}

/**
 * @brief  设置IP地址
 * @param  ip_addr       : 输入参数, IP地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool set_ip_addr(const char *ip_addr, const char *interface_name)
{
    assert(NULL != ip_addr);
    assert(NULL != interface_name);

    int fd = -1;
    struct ifreq ifr = {0};

    // 打开一个socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        return false;
    }

    // 设置要修改的网络接口名
    strcpy(ifr.ifr_name, interface_name);

    // 设置新的IP地址和子网掩码
    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = AF_INET;
    inet_pton(AF_INET, ip_addr, &addr->sin_addr);

    // 设置新的IP地址
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
    {
        close(fd);

        return false;
    }

    // 关闭socket
    close(fd);

    return true;
}

/**
 * @brief  获取子网掩码
 * @param  netmask       : 输出参数, 子网掩码
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_netmask(char *netmask, const char *interface_name)
{
    assert(NULL != netmask);
    assert(NULL != interface_name);

    int fd = -1;
    struct ifreq ifr = {0};

    // 打开一个socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        return false;
    }

    // 设置要查询的网络接口名
    strcpy(ifr.ifr_name, interface_name);

    // 查询子网掩码
    if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0)
    {
        close(fd);

        return false;
    }
    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_netmask;
    inet_ntop(AF_INET, &addr->sin_addr, netmask, INET_ADDRSTRLEN);

    // 关闭socket
    close(fd);

    return true;
}

/**
 * @brief  设置子网掩码
 * @param  netmask       : 输入参数, 子网掩码
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool set_netmask(const char *netmask, const char *interface_name)
{
    assert(NULL != netmask);
    assert(NULL != interface_name);

    int fd = -1;
    struct ifreq ifr = {0};

    // 打开一个socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        return false;
    }

    // 设置要修改的网络接口名
    strcpy(ifr.ifr_name, interface_name);

    // 设置新的子网掩码
    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_netmask;
    addr->sin_family = AF_INET;
    inet_pton(AF_INET, netmask, &addr->sin_addr);

    // 使用ioctl调用来设置新的子网掩码
    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
    {
        close(fd);

        return false;
    }

    // 关闭socket
    close(fd);

    return true;
}

/**
 * @brief  点分格式子网掩码转换为CIDR格式
 * @param  cidr   : 输出参数, CIDR格式子网掩码
 * @param  netmask: 输入参数, 点分格式子网掩码
 * @return true : 成功
 * @return false: 失败
 */
bool netmask_to_cidr(uint8_t *cidr, const char *netmask)
{
    assert(NULL != cidr);
    assert(NULL != netmask);

    struct in_addr addr = {0};
    if (1 == inet_pton(AF_INET, netmask, &addr))
    {
        uint32_t netmask_num = ntohl(addr.s_addr);
        while (netmask_num)
        {
            *cidr += netmask_num & 1;
            netmask_num >>= 1;
        }

        return true;
    }

    return false;
}

/**
 * @brief  CIDR格式子网掩码转换为点分格式
 * @param  netmask: 输出参数, 点分格式子网掩码
 * @param  cidr   : 输入参数, CIDR格式子网掩码
 * @return true : 成功
 * @return false: 失败
 */
bool cidr_to_netmask(char *netmask, const uint8_t cidr)
{
    assert(NULL != netmask);

    if ((cidr >= 0) && (cidr <= 32))
    {
        uint32_t netmask_num = htonl(0xFFFFFFFF << (32 - cidr));

        struct in_addr addr = {0};
        addr.s_addr = netmask_num;
        snprintf(netmask, INET_ADDRSTRLEN, "%s", inet_ntoa(addr));

        return true;
    }

    return false;
}

/**
 * @brief  获取MAC地址
 * @param  mac_addr      : 输出参数, MAC地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_mac_addr(char *mac_addr, const char *interface_name)
{
    assert(NULL != mac_addr);
    assert(NULL != interface_name);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        return false;
    }

    struct ifreq ifr = {0};
    strncpy(ifr.ifr_name, interface_name, (IFNAMSIZ - 1));
    ifr.ifr_addr.sa_family = AF_INET;

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    {
        close(fd);

        return false;
    }

    close(fd);

    sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x", (uint8_t)ifr.ifr_hwaddr.sa_data[0],
            (uint8_t)ifr.ifr_hwaddr.sa_data[1], (uint8_t)ifr.ifr_hwaddr.sa_data[2], (uint8_t)ifr.ifr_hwaddr.sa_data[3],
            (uint8_t)ifr.ifr_hwaddr.sa_data[4], (uint8_t)ifr.ifr_hwaddr.sa_data[5]);

    return true;
}

/**
 * @brief  获取网关地址
 * @param  gateway_addr  : 输出参数, 网关地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_gateway_addr(char *gateway_addr, const char *interface_name)
{
    assert(NULL != gateway_addr);
    assert(NULL != interface_name);

    FILE *fp = fopen("/proc/net/route", "r");
    if (fp == NULL)
    {
        return false;
    }

    // 跳过表头
    char line[256] = "";
    fgets(line, sizeof(line), fp);

    // 逐行读取路由表
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char iface[16] = "";
        uint32_t destination = 0;
        uint32_t gateway = 0;
        if (sscanf(line, "%15s %d %d", iface, &destination, &gateway) == 3)
        {
            // destination为0: 默认网关
            if ((strcmp(iface, interface_name) == 0) && (destination == 0))
            {
                snprintf(gateway_addr, INET_ADDRSTRLEN, "%s", inet_ntoa(*(struct in_addr *)&gateway));
                fclose(fp);

                return true;
            }
        }
    }

    fclose(fp);

    return true;
}

/**
 * @brief  ping主机
 * @param  hostname  : 输入参数, 主机地址(域名或IP)
 * @param  ping_count: 输入参数, ping的次数
 * @return true : 成功
 * @return false: 失败
 */
bool ping_host(const char *hostname, const uint8_t ping_count)
{
    struct addrinfo hints = {0};
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    struct addrinfo *result;
    int ret = getaddrinfo(hostname, NULL, &hints, &result);
    if (ret != 0)
    {
        return false;
    }

    struct sockaddr ping_addr;
    memcpy(&ping_addr, result->ai_addr, sizeof(struct sockaddr));
    freeaddrinfo(result);

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (fd < 0)
    {
        return false;
    }

    // 设置套接字超时
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    if (ret < 0)
    {
        close(fd);

        return false;
    }

    for (uint8_t i = 1; i <= ping_count; i++)
    {
        if (!ping(fd, &ping_addr, i))
        {
            close(fd);

            return false;
        }
    }

    close(fd);

    return true;
}
