/*
 Navicat Premium Data Transfer

 Source Server         : vmec
 Source Server Type    : SQLite
 Source Server Version : 3035005 (3.35.5)
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3035005 (3.35.5)
 File Encoding         : 65001

 Date: 16/08/2026 02:42:28
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for metadata
-- ----------------------------
DROP TABLE IF EXISTS "metadata";
CREATE TABLE "metadata" (
  "key" TEXT,
  "value" TEXT NOT NULL,
  "updated_at" TEXT DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
  PRIMARY KEY ("key")
);

-- ----------------------------
-- Records of metadata
-- ----------------------------
INSERT INTO "metadata" VALUES ('input_hash', '6f274a094265889ae7c615b1ea69d5e6b017352ce28dfd7daed64cfb67f6ec68', '2026-06-30T16:10:01.854Z');

PRAGMA foreign_keys = true;
