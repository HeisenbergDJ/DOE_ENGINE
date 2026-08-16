/*
 Navicat Premium Data Transfer

 Source Server         : vmec
 Source Server Type    : SQLite
 Source Server Version : 3035005 (3.35.5)
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3035005 (3.35.5)
 File Encoding         : 65001

 Date: 16/08/2026 02:42:46
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for task_status
-- ----------------------------
DROP TABLE IF EXISTS "task_status";
CREATE TABLE "task_status" (
  "task_id" INTEGER,
  "status" TEXT NOT NULL DEFAULT 'PENDING',
  "retry_count" INTEGER NOT NULL DEFAULT 0,
  "assigned_rank" INTEGER,
  "started_at" TEXT,
  "finished_at" TEXT,
  "elapsed_ms" REAL,
  "error_code" INTEGER,
  "error_message" TEXT,
  "updated_at" TEXT DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
  PRIMARY KEY ("task_id"),
  FOREIGN KEY ("task_id") REFERENCES "experiments" ("task_id") ON DELETE NO ACTION ON UPDATE NO ACTION
);

-- ----------------------------
-- Records of task_status
-- ----------------------------
INSERT INTO "task_status" VALUES (1, 'FAILED', 1, 1, '2026-06-30T16:10:01.890Z', '2026-06-30T16:11:33.849Z', 4.17369417846203, 1, 'Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 42, in read
    with Dataset(nc_path, mode="r") as ds:
  File "src/netCDF4/_netCDF4.pyx", line 2522, in netCDF4._netCDF4.Dataset.__init__
  File "src/netCDF4/_netCDF4.pyx", line 2159, in netCDF4._netCDF4._ensure_nc_success
FileNotFoundError: [Errno 2] No such file or directory: ''data/nc/wout_2b40Ncoil16_B01opt_free_opt1.nc''

The above exception was the direct cause of the following exception:

Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 59, in worker_fn
    metric_result, func_results = _execute(task_msg)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 113, in _execute
    nc_data = nc_reader.read(nc_path, required_vars)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 51, in read
    raise RuntimeError(f"nc file not found: {nc_path}") from exc
RuntimeError: nc file not found: data/nc/wout_2b40Ncoil16_B01opt_free_opt1.nc
', '2026-06-30T16:11:33.849Z');
INSERT INTO "task_status" VALUES (2, 'SUCCESS', 0, 2, '2026-06-30T16:10:01.891Z', '2026-06-30T16:11:35.244Z', 92291.2003155798, NULL, NULL, '2026-06-30T16:11:35.244Z');
INSERT INTO "task_status" VALUES (3, 'SUCCESS', 0, 3, '2026-06-30T16:10:01.893Z', '2026-06-30T16:11:35.246Z', 92625.0332556665, NULL, NULL, '2026-06-30T16:11:35.246Z');
INSERT INTO "task_status" VALUES (4, 'SUCCESS', 0, 4, '2026-06-30T16:10:01.894Z', '2026-06-30T16:11:35.252Z', 93345.2993817627, NULL, NULL, '2026-06-30T16:11:35.252Z');
INSERT INTO "task_status" VALUES (5, 'SUCCESS', 0, 5, '2026-06-30T16:10:01.896Z', '2026-06-30T16:11:35.247Z', 92756.4751245081, NULL, NULL, '2026-06-30T16:11:35.247Z');
INSERT INTO "task_status" VALUES (6, 'SUCCESS', 0, 6, '2026-06-30T16:10:01.897Z', '2026-06-30T16:11:35.244Z', 92301.4629036188, NULL, NULL, '2026-06-30T16:11:35.244Z');
INSERT INTO "task_status" VALUES (7, 'SUCCESS', 0, 7, '2026-06-30T16:10:01.899Z', '2026-06-30T16:11:35.246Z', 92621.6408349574, NULL, NULL, '2026-06-30T16:11:35.246Z');
INSERT INTO "task_status" VALUES (8, 'SUCCESS', 0, 8, '2026-06-30T16:10:01.900Z', '2026-06-30T16:11:35.251Z', 93128.07113491, NULL, NULL, '2026-06-30T16:11:35.251Z');
INSERT INTO "task_status" VALUES (9, 'SUCCESS', 0, 9, '2026-06-30T16:10:01.902Z', '2026-06-30T16:11:35.250Z', 92987.091075629, NULL, NULL, '2026-06-30T16:11:35.250Z');
INSERT INTO "task_status" VALUES (10, 'FAILED', 1, 10, '2026-06-30T16:10:01.903Z', '2026-06-30T16:11:33.850Z', 0.69555826485157, 1, 'Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 42, in read
    with Dataset(nc_path, mode="r") as ds:
  File "src/netCDF4/_netCDF4.pyx", line 2522, in netCDF4._netCDF4.Dataset.__init__
  File "src/netCDF4/_netCDF4.pyx", line 2159, in netCDF4._netCDF4._ensure_nc_success
FileNotFoundError: [Errno 2] No such file or directory: ''data/nc/wout_2b40Ncoil16_B01opt_free_opt10.nc''

The above exception was the direct cause of the following exception:

Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 59, in worker_fn
    metric_result, func_results = _execute(task_msg)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 113, in _execute
    nc_data = nc_reader.read(nc_path, required_vars)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 51, in read
    raise RuntimeError(f"nc file not found: {nc_path}") from exc
RuntimeError: nc file not found: data/nc/wout_2b40Ncoil16_B01opt_free_opt10.nc
', '2026-06-30T16:11:33.850Z');
INSERT INTO "task_status" VALUES (11, 'SUCCESS', 0, 11, '2026-06-30T16:10:01.905Z', '2026-06-30T16:11:35.251Z', 93147.6765237749, NULL, NULL, '2026-06-30T16:11:35.251Z');
INSERT INTO "task_status" VALUES (12, 'SUCCESS', 0, 12, '2026-06-30T16:10:01.906Z', '2026-06-30T16:11:35.248Z', 92783.5428435355, NULL, NULL, '2026-06-30T16:11:35.248Z');
INSERT INTO "task_status" VALUES (13, 'SUCCESS', 0, 13, '2026-06-30T16:10:01.908Z', '2026-06-30T16:11:33.850Z', 91937.5382717699, NULL, NULL, '2026-06-30T16:11:33.850Z');
INSERT INTO "task_status" VALUES (14, 'SUCCESS', 0, 14, '2026-06-30T16:10:01.909Z', '2026-06-30T16:11:35.249Z', 92877.0658187568, NULL, NULL, '2026-06-30T16:11:35.249Z');
INSERT INTO "task_status" VALUES (15, 'SUCCESS', 0, 15, '2026-06-30T16:10:01.911Z', '2026-06-30T16:11:35.252Z', 93271.9475626945, NULL, NULL, '2026-06-30T16:11:35.252Z');
INSERT INTO "task_status" VALUES (16, 'SUCCESS', 0, 16, '2026-06-30T16:10:01.912Z', '2026-06-30T16:11:35.249Z', 92812.7840720117, NULL, NULL, '2026-06-30T16:11:35.249Z');
INSERT INTO "task_status" VALUES (17, 'SUCCESS', 0, 17, '2026-06-30T16:10:01.914Z', '2026-06-30T16:11:35.245Z', 92458.7195795029, NULL, NULL, '2026-06-30T16:11:35.245Z');
INSERT INTO "task_status" VALUES (18, 'SUCCESS', 0, 18, '2026-06-30T16:10:01.915Z', '2026-06-30T16:11:35.247Z', 92635.1221315563, NULL, NULL, '2026-06-30T16:11:35.247Z');
INSERT INTO "task_status" VALUES (19, 'FAILED', 1, 19, '2026-06-30T16:10:01.916Z', '2026-06-30T16:11:33.850Z', 0.741966068744659, 1, 'Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 42, in read
    with Dataset(nc_path, mode="r") as ds:
  File "src/netCDF4/_netCDF4.pyx", line 2522, in netCDF4._netCDF4.Dataset.__init__
  File "src/netCDF4/_netCDF4.pyx", line 2159, in netCDF4._netCDF4._ensure_nc_success
FileNotFoundError: [Errno 2] No such file or directory: ''data/nc/wout_2b40Ncoil16_B01opt_free_opt19.nc''

The above exception was the direct cause of the following exception:

Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 59, in worker_fn
    metric_result, func_results = _execute(task_msg)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 113, in _execute
    nc_data = nc_reader.read(nc_path, required_vars)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 51, in read
    raise RuntimeError(f"nc file not found: {nc_path}") from exc
RuntimeError: nc file not found: data/nc/wout_2b40Ncoil16_B01opt_free_opt19.nc
', '2026-06-30T16:11:33.850Z');
INSERT INTO "task_status" VALUES (20, 'SUCCESS', 0, 20, '2026-06-30T16:10:01.918Z', '2026-06-30T16:11:35.245Z', 92396.0129264742, NULL, NULL, '2026-06-30T16:11:35.245Z');
INSERT INTO "task_status" VALUES (21, 'SUCCESS', 0, 21, '2026-06-30T16:10:01.919Z', '2026-06-30T16:11:35.247Z', 92618.8021507114, NULL, NULL, '2026-06-30T16:11:35.247Z');
INSERT INTO "task_status" VALUES (22, 'SUCCESS', 0, 22, '2026-06-30T16:10:01.921Z', '2026-06-30T16:11:35.248Z', 92733.5186284035, NULL, NULL, '2026-06-30T16:11:35.248Z');
INSERT INTO "task_status" VALUES (23, 'SUCCESS', 0, 23, '2026-06-30T16:10:01.922Z', '2026-06-30T16:11:35.244Z', 92306.4879756421, NULL, NULL, '2026-06-30T16:11:35.244Z');
INSERT INTO "task_status" VALUES (24, 'SUCCESS', 0, 24, '2026-06-30T16:10:01.924Z', '2026-06-30T16:11:35.246Z', 92514.5600195974, NULL, NULL, '2026-06-30T16:11:35.246Z');
INSERT INTO "task_status" VALUES (25, 'SUCCESS', 0, 25, '2026-06-30T16:10:01.925Z', '2026-06-30T16:11:35.243Z', 92243.7667120248, NULL, NULL, '2026-06-30T16:11:35.243Z');
INSERT INTO "task_status" VALUES (26, 'SUCCESS', 0, 26, '2026-06-30T16:10:01.927Z', '2026-06-30T16:11:35.250Z', 92867.6034063101, NULL, NULL, '2026-06-30T16:11:35.250Z');
INSERT INTO "task_status" VALUES (27, 'SUCCESS', 0, 27, '2026-06-30T16:10:01.928Z', '2026-06-30T16:11:35.249Z', 92846.7915318906, NULL, NULL, '2026-06-30T16:11:35.249Z');
INSERT INTO "task_status" VALUES (28, 'FAILED', 1, 28, '2026-06-30T16:10:01.930Z', '2026-06-30T16:11:33.850Z', 0.905295833945274, 1, 'Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 42, in read
    with Dataset(nc_path, mode="r") as ds:
  File "src/netCDF4/_netCDF4.pyx", line 2522, in netCDF4._netCDF4.Dataset.__init__
  File "src/netCDF4/_netCDF4.pyx", line 2159, in netCDF4._netCDF4._ensure_nc_success
FileNotFoundError: [Errno 2] No such file or directory: ''data/nc/wout_2b40Ncoil16_B01opt_free_opt28.nc''

The above exception was the direct cause of the following exception:

Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 59, in worker_fn
    metric_result, func_results = _execute(task_msg)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 113, in _execute
    nc_data = nc_reader.read(nc_path, required_vars)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 51, in read
    raise RuntimeError(f"nc file not found: {nc_path}") from exc
RuntimeError: nc file not found: data/nc/wout_2b40Ncoil16_B01opt_free_opt28.nc
', '2026-06-30T16:11:33.850Z');
INSERT INTO "task_status" VALUES (29, 'SUCCESS', 0, 29, '2026-06-30T16:10:01.931Z', '2026-06-30T16:11:35.244Z', 92352.2924073041, NULL, NULL, '2026-06-30T16:11:35.244Z');
INSERT INTO "task_status" VALUES (30, 'SUCCESS', 0, 30, '2026-06-30T16:10:01.933Z', '2026-06-30T16:11:35.251Z', 93107.1504913271, NULL, NULL, '2026-06-30T16:11:35.251Z');
INSERT INTO "task_status" VALUES (31, 'SUCCESS', 0, 31, '2026-06-30T16:10:01.934Z', '2026-06-30T16:11:35.246Z', 92536.3536775112, NULL, NULL, '2026-06-30T16:11:35.246Z');
INSERT INTO "task_status" VALUES (32, 'SUCCESS', 0, 32, '2026-06-30T16:10:01.935Z', '2026-06-30T16:11:35.250Z', 92852.7152892202, NULL, NULL, '2026-06-30T16:11:35.250Z');
INSERT INTO "task_status" VALUES (33, 'SUCCESS', 0, 33, '2026-06-30T16:10:01.937Z', '2026-06-30T16:11:35.251Z', 93116.3670532405, NULL, NULL, '2026-06-30T16:11:35.251Z');
INSERT INTO "task_status" VALUES (34, 'SUCCESS', 0, 34, '2026-06-30T16:10:01.938Z', '2026-06-30T16:11:35.243Z', 91906.7262094468, NULL, NULL, '2026-06-30T16:11:35.243Z');
INSERT INTO "task_status" VALUES (35, 'SUCCESS', 0, 35, '2026-06-30T16:10:01.940Z', '2026-06-30T16:11:35.248Z', 92727.3211628199, NULL, NULL, '2026-06-30T16:11:35.248Z');
INSERT INTO "task_status" VALUES (36, 'SUCCESS', 0, 36, '2026-06-30T16:10:01.941Z', '2026-06-30T16:11:35.252Z', 93126.0879039764, NULL, NULL, '2026-06-30T16:11:35.252Z');
INSERT INTO "task_status" VALUES (37, 'FAILED', 1, 37, '2026-06-30T16:10:01.943Z', '2026-06-30T16:11:33.850Z', 0.70720911026001, 1, 'Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 42, in read
    with Dataset(nc_path, mode="r") as ds:
  File "src/netCDF4/_netCDF4.pyx", line 2522, in netCDF4._netCDF4.Dataset.__init__
  File "src/netCDF4/_netCDF4.pyx", line 2159, in netCDF4._netCDF4._ensure_nc_success
FileNotFoundError: [Errno 2] No such file or directory: ''data/nc/wout_2b40Ncoil16_B01opt_free_opt37.nc''

The above exception was the direct cause of the following exception:

Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 59, in worker_fn
    metric_result, func_results = _execute(task_msg)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 113, in _execute
    nc_data = nc_reader.read(nc_path, required_vars)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 51, in read
    raise RuntimeError(f"nc file not found: {nc_path}") from exc
RuntimeError: nc file not found: data/nc/wout_2b40Ncoil16_B01opt_free_opt37.nc
', '2026-06-30T16:11:33.850Z');
INSERT INTO "task_status" VALUES (38, 'SUCCESS', 0, 38, '2026-06-30T16:10:01.944Z', '2026-06-30T16:11:35.247Z', 92685.1299144328, NULL, NULL, '2026-06-30T16:11:35.247Z');
INSERT INTO "task_status" VALUES (39, 'SUCCESS', 0, 39, '2026-06-30T16:10:01.946Z', '2026-06-30T16:11:35.243Z', 92109.1510560364, NULL, NULL, '2026-06-30T16:11:35.243Z');
INSERT INTO "task_status" VALUES (40, 'SUCCESS', 0, 40, '2026-06-30T16:10:03.474Z', '2026-06-30T16:11:35.245Z', 90836.3867998123, NULL, NULL, '2026-06-30T16:11:35.245Z');
INSERT INTO "task_status" VALUES (41, 'SUCCESS', 0, 41, '2026-06-30T16:10:03.538Z', '2026-06-30T16:11:35.248Z', 91148.4078802168, NULL, NULL, '2026-06-30T16:11:35.248Z');
INSERT INTO "task_status" VALUES (42, 'SUCCESS', 0, 42, '2026-06-30T16:10:03.540Z', '2026-06-30T16:11:35.249Z', 91238.3732683957, NULL, NULL, '2026-06-30T16:11:35.249Z');
INSERT INTO "task_status" VALUES (43, 'SUCCESS', 0, 43, '2026-06-30T16:10:03.541Z', '2026-06-30T16:11:35.243Z', 90580.1458861679, NULL, NULL, '2026-06-30T16:11:35.243Z');
INSERT INTO "task_status" VALUES (44, 'SUCCESS', 0, 44, '2026-06-30T16:10:03.543Z', '2026-06-30T16:11:35.251Z', 91358.9704819024, NULL, NULL, '2026-06-30T16:11:35.251Z');
INSERT INTO "task_status" VALUES (45, 'SUCCESS', 0, 45, '2026-06-30T16:10:03.545Z', '2026-06-30T16:11:35.250Z', 91301.9733782858, NULL, NULL, '2026-06-30T16:11:35.250Z');
INSERT INTO "task_status" VALUES (46, 'FAILED', 1, 46, '2026-06-30T16:10:03.547Z', '2026-06-30T16:11:33.850Z', 578.248763456941, 1, 'Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 42, in read
    with Dataset(nc_path, mode="r") as ds:
  File "src/netCDF4/_netCDF4.pyx", line 2522, in netCDF4._netCDF4.Dataset.__init__
  File "src/netCDF4/_netCDF4.pyx", line 2159, in netCDF4._netCDF4._ensure_nc_success
FileNotFoundError: [Errno 2] No such file or directory: ''data/nc/wout_2b40Ncoil16_B01opt_free_opt46.nc''

The above exception was the direct cause of the following exception:

Traceback (most recent call last):
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 59, in worker_fn
    metric_result, func_results = _execute(task_msg)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/worker.py", line 113, in _execute
    nc_data = nc_reader.read(nc_path, required_vars)
  File "/public/home/cfqs_hfliu/Heisenberg/projects/vmec-dataprocess-mpi-release-v3/nc_reader.py", line 51, in read
    raise RuntimeError(f"nc file not found: {nc_path}") from exc
RuntimeError: nc file not found: data/nc/wout_2b40Ncoil16_B01opt_free_opt46.nc
', '2026-06-30T16:11:33.850Z');
INSERT INTO "task_status" VALUES (47, 'SUCCESS', 0, 47, '2026-06-30T16:10:03.549Z', '2026-06-30T16:11:35.245Z', 90593.1019838899, NULL, NULL, '2026-06-30T16:11:35.245Z');
INSERT INTO "task_status" VALUES (48, 'SUCCESS', 0, 48, '2026-06-30T16:10:03.984Z', '2026-06-30T16:11:35.248Z', 90649.7600413859, NULL, NULL, '2026-06-30T16:11:35.248Z');
INSERT INTO "task_status" VALUES (49, 'SUCCESS', 0, 49, '2026-06-30T16:10:04.412Z', '2026-06-30T16:11:35.245Z', 88602.7340125293, NULL, NULL, '2026-06-30T16:11:35.245Z');
INSERT INTO "task_status" VALUES (50, 'SUCCESS', 0, 1, '2026-06-30T16:10:11.847Z', '2026-06-30T16:11:35.252Z', 70591.2147164345, NULL, NULL, '2026-06-30T16:11:35.252Z');

-- ----------------------------
-- Indexes structure for table task_status
-- ----------------------------
CREATE INDEX "idx_task_status_retry"
ON "task_status" (
  "status" ASC,
  "retry_count" ASC
);
CREATE INDEX "idx_task_status_status"
ON "task_status" (
  "status" ASC
);

PRAGMA foreign_keys = true;
