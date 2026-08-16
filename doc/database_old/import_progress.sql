/*
 Navicat Premium Data Transfer

 Source Server         : vmec
 Source Server Type    : SQLite
 Source Server Version : 3035005 (3.35.5)
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3035005 (3.35.5)
 File Encoding         : 65001

 Date: 16/08/2026 02:42:11
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for import_progress
-- ----------------------------
DROP TABLE IF EXISTS "import_progress";
CREATE TABLE "import_progress" (
  "import_name" TEXT,
  "map_file" TEXT NOT NULL,
  "last_line_no" INTEGER NOT NULL DEFAULT 0,
  "last_byte_offset" INTEGER NOT NULL DEFAULT 0,
  "is_completed" INTEGER NOT NULL DEFAULT 0,
  "updated_at" TEXT DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
  PRIMARY KEY ("import_name")
);

-- ----------------------------
-- Records of import_progress
-- ----------------------------
INSERT INTO "import_progress" VALUES ('main', 'map.dat', 51, 0, 1, '2026-06-30T16:10:01.862Z');

PRAGMA foreign_keys = true;
