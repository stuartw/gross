/* Copyright Abandoned 1996, 1999, 2001 MySQL AB
   This file is public domain and comes with NO WARRANTY of any kind */

/* Version numbers for protocol & mysqld */

#ifndef _mysql_version_h
#define _mysql_version_h
#ifdef _CUSTOMCONFIG_
#include <custom_conf.h>
#else
#define PROTOCOL_VERSION		10
#define MYSQL_SERVER_VERSION		"4.0.12"
#ifndef MYSQL_SERVER_SUFFIX
#define MYSQL_SERVER_SUFFIX		""
#endif
#define FRM_VER				6
#define MYSQL_VERSION_ID		40012
#define MYSQL_PORT			3306
#define MYSQL_UNIX_ADDR			"/var/lib/mysql/mysql.sock"
#define MYSQL_CONFIG_NAME		"my"

/* mysqld compile time options */
#ifndef MYSQL_CHARSET
#define MYSQL_CHARSET			"latin1"
#endif /* MYSQL_CHARSET */
#endif /* _CUSTOMCONFIG_ */
#endif /* _mysql_version_h */
