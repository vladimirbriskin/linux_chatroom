/*
 Navicat Premium Data Transfer

 Source Server         : chatroom
 Source Server Type    : MySQL
 Source Server Version : 80025
 Source Host           : 127.0.0.1:3306
 Source Schema         : mychatroom

 Target Server Type    : MySQL
 Target Server Version : 80025
 File Encoding         : 65001

 Date: 08/07/2021 23:17:49
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for filemng
-- ----------------------------
DROP TABLE IF EXISTS `filemng`;
CREATE TABLE `filemng` (
  `fsdr` varchar(255) DEFAULT NULL,
  `frcv` varchar(255) DEFAULT NULL,
  `fid` int unsigned NOT NULL AUTO_INCREMENT,
  `size` int DEFAULT NULL,
  `fname` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Table structure for friend
-- ----------------------------
DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `fuid` int NOT NULL,
  `uid` int unsigned NOT NULL,
  PRIMARY KEY (`uid`,`fuid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Table structure for group_member
-- ----------------------------
DROP TABLE IF EXISTS `group_member`;
CREATE TABLE `group_member` (
  `gid` int NOT NULL,
  `uid` int NOT NULL,
  PRIMARY KEY (`gid`,`uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Table structure for group_recv
-- ----------------------------
DROP TABLE IF EXISTS `group_recv`;
CREATE TABLE `group_recv` (
  `gid` int NOT NULL,
  `uname` varchar(255) NOT NULL,
  `msg` varchar(255) NOT NULL,
  `mid` int unsigned NOT NULL AUTO_INCREMENT COMMENT '消息的序号，唯一',
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Table structure for grp
-- ----------------------------
DROP TABLE IF EXISTS `grp`;
CREATE TABLE `grp` (
  `gid` int unsigned NOT NULL AUTO_INCREMENT,
  `lordid` int NOT NULL,
  `gname` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`gid`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Table structure for private_recv
-- ----------------------------
DROP TABLE IF EXISTS `private_recv`;
CREATE TABLE `private_recv` (
  `from_uid` int NOT NULL,
  `to_uid` int NOT NULL,
  `msg` varchar(255) NOT NULL,
  `mid` int unsigned NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`mid`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Table structure for user_info
-- ----------------------------
DROP TABLE IF EXISTS `user_info`;
CREATE TABLE `user_info` (
  `uid` int unsigned NOT NULL AUTO_INCREMENT,
  `password` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '123456',
  `name` varchar(255) NOT NULL,
  PRIMARY KEY (`uid`,`name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb3;

SET FOREIGN_KEY_CHECKS = 1;
