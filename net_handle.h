/**
 * @file      : net_handle.h
 * @brief     : Linux平台网络处理函数接口头文件
 * @author    : huenrong (huenrong1028@outlook.com)
 * @date      : 2023-08-19 12:01:15
 *
 * @copyright : Copyright (c) 2023 huenrong
 *
 * @history   : date        author          description
 *              2023-08-19  huenrong        创建文件
 *
 */

#ifndef __NET_HANDLE_H
#define __NET_HANDLE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief  获取IP地址
 * @param  ip_addr       : 输出参数, IP地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_ip_addr(char *ip_addr, const char *interface_name);

/**
 * @brief  设置IP地址
 * @param  ip_addr       : 输入参数, IP地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool set_ip_addr(const char *ip_addr, const char *interface_name);

/**
 * @brief  获取子网掩码
 * @param  netmask       : 输出参数, 子网掩码
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_netmask(char *netmask, const char *interface_name);

/**
 * @brief  设置子网掩码
 * @param  netmask       : 输入参数, 子网掩码
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool set_netmask(const char *netmask, const char *interface_name);

/**
 * @brief  点分格式子网掩码转换为CIDR格式
 * @param  cidr   : 输出参数, CIDR格式子网掩码
 * @param  netmask: 输入参数, 点分格式子网掩码
 * @return true : 成功
 * @return false: 失败
 */
bool netmask_to_cidr(uint8_t *cidr, const char *netmask);

/**
 * @brief  CIDR格式子网掩码转换为点分格式
 * @param  netmask: 输出参数, 点分格式子网掩码
 * @param  cidr   : 输入参数, CIDR格式子网掩码
 * @return true : 成功
 * @return false: 失败
 */
bool cidr_to_netmask(char *netmask, const uint8_t cidr);

/**
 * @brief  获取MAC地址
 * @param  mac_addr      : 输出参数, MAC地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_mac_addr(char *mac_addr, const char *interface_name);

/**
 * @brief  获取网关地址
 * @param  gateway_addr  : 输出参数, 网关地址
 * @param  interface_name: 输入参数, 网络接口名
 * @return true : 成功
 * @return false: 失败
 */
bool get_gateway_addr(char *gateway_addr, const char *interface_name);

/**
 * @brief  ping主机
 * @param  hostname  : 输入参数, 主机地址(域名或IP)
 * @param  ping_count: 输入参数, ping的次数
 * @return true : 成功
 * @return false: 失败
 */
bool ping_host(const char *hostname, const uint8_t ping_count);

#ifdef __cplusplus
}
#endif

#endif // __NET_HANDLE_H
