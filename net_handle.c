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
