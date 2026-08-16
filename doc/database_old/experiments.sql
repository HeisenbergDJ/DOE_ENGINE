/*
 Navicat Premium Data Transfer

 Source Server         : vmec
 Source Server Type    : SQLite
 Source Server Version : 3035005 (3.35.5)
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3035005 (3.35.5)
 File Encoding         : 65001

 Date: 16/08/2026 02:41:48
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for experiments
-- ----------------------------
DROP TABLE IF EXISTS "experiments";
CREATE TABLE "experiments" (
  "task_id" INTEGER,
  "map_line_no" INTEGER NOT NULL,
  "nc_path" TEXT NOT NULL,
  "design_vars_json" TEXT NOT NULL,
  "created_at" TEXT DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
  PRIMARY KEY ("task_id")
);

-- ----------------------------
-- Records of experiments
-- ----------------------------
INSERT INTO "experiments" VALUES (1, 2, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt1.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (2, 3, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt2.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (3, 4, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt3.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (4, 5, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt4.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (5, 6, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt5.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 0.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (6, 7, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt6.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (7, 8, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt7.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (8, 9, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt8.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (9, 10, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt9.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (10, 11, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt10.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -23437.5, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (11, 12, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt11.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (12, 13, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt12.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (13, 14, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt13.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (14, 15, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt14.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (15, 16, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt15.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 0.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (16, 17, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt16.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (17, 18, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt17.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (18, 19, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt18.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (19, 20, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt19.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (20, 21, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt20.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -23437.5, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (21, 22, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt21.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (22, 23, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt22.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (23, 24, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt23.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (24, 25, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt24.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (25, 26, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt25.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 0.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (26, 27, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt26.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (27, 28, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt27.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (28, 29, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt28.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (29, 30, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt29.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (30, 31, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt30.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -23437.5, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (31, 32, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt31.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (32, 33, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt32.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (33, 34, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt33.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (34, 35, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt34.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (35, 36, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt35.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 0.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (36, 37, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt36.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (37, 38, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt37.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (38, 39, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt38.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (39, 40, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt39.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (40, 41, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt40.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -23437.5, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (41, 42, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt41.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (42, 43, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt42.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (43, 44, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt43.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (44, 45, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt44.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": -50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (45, 46, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt45.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 0.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (46, 47, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt46.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 50000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (47, 48, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt47.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 100000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (48, 49, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt48.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 150000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (49, 50, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt49.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -31250.0, "v4": 200000.0}', '2026-06-30T16:10:01.860Z');
INSERT INTO "experiments" VALUES (50, 51, 'data/nc/wout_2b40Ncoil16_B01opt_free_opt50.nc', '{"v1": -31250.0, "v2": -31250.0, "v3": -23437.5, "v4": -200000.0}', '2026-06-30T16:10:01.860Z');

-- ----------------------------
-- Indexes structure for table experiments
-- ----------------------------
CREATE INDEX "idx_experiments_path"
ON "experiments" (
  "nc_path" ASC
);

PRAGMA foreign_keys = true;
