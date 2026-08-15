#!/usr/bin/env python3
"""Extract the scalar Q metric from a VMEC wout NetCDF file."""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path

from metric_algo.unified_metrics import compute_all_unified_metrics


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--wout", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--nthetastar", type=int, required=True)
    parser.add_argument("--nzeta", type=int, required=True)
    parser.add_argument("--delta-ntheta", type=int, required=True)
    parser.add_argument("--delta-nzeta", type=int, required=True)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    metrics = compute_all_unified_metrics(
        nc_path=str(args.wout),
        nthetastar=args.nthetastar,
        nzeta=args.nzeta,
        delta_ntheta=args.delta_ntheta,
        delta_nzeta=args.delta_nzeta,
    )
    q = float(metrics.get("Q_scalar_vmec", metrics.get("epsilon_qa")))
    if not math.isfinite(q):
        raise ValueError(f"Q is not finite: {q}")
    args.output.write_text(
        json.dumps({"q": q}, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
