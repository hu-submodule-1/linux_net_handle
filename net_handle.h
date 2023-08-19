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

#ifdef __cplusplus
}
#endif

#endif // __NET_HANDLE_H
