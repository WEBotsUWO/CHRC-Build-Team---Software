#!/usr/bin/env python3
"""Launch the AM-ARM200 LeRobot fork's interactive CHRC follower calibration.

Requires: an activated environment with liyiteng/lerobot_alohamini installed,
           and an 'am-chrc-6dof' arm profile added to so_follower.py.

Usage:
    python start_chrc_calibration.py --port COM5 --dry-run
    python start_chrc_calibration.py --port COM5
"""

import argparse
import importlib
import shlex
import subprocess
import sys

PROFILE = "am-chrc-6dof"  # TODO: add this six-servo profile in so_follower.py
ROBOT_ID = "chrc_follower"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="Serial port, e.g. COM5 or /dev/ttyACM0")
    parser.add_argument("--dry-run", action="store_true", help="Show command only; do not connect")
    args = parser.parse_args()

    command = [
        sys.executable, "-m", "lerobot.scripts.lerobot_calibrate",
        "--robot.type=so101_follower",
        f"--robot.port={args.port}",
        f"--robot.id={ROBOT_ID}",
        f"--robot.arm_profile={PROFILE}",
    ]

    print("\nCHRC follower-arm calibration")
    print("Command:", shlex.join(command))
    if args.dry_run:
        print("DRY RUN: no hardware accessed.")
        return 0

    # Fail before connecting if the six-motor profile hasn't been installed.
    try:
        driver = importlib.import_module("lerobot.robots.so_follower.so_follower")
    except ImportError as exc:
        print(f"ERROR: LeRobot fork isn't importable in this Python environment: {exc}", file=sys.stderr)
        print("Install the AM-ARM200 fork and activate its environment.", file=sys.stderr)
        return 1
    profiles = getattr(driver, "_ARM_PROFILES", {})
    if PROFILE not in profiles:
        print(f"ERROR: profile '{PROFILE}' is missing from so_follower.py.", file=sys.stderr)
        print("Copy 'am-follower-6dof' and remove the original gripper (ID 7).", file=sys.stderr)
        return 1
    configured_ids = [motor[1] for motor in profiles[PROFILE]]
    if configured_ids != [1, 2, 3, 4, 5, 6]:
        print(f"ERROR: unexpected motor IDs in profile: {configured_ids}", file=sys.stderr)
        return 1

    print("\nSAFETY CHECK BEFORE CONNECTING:")
    print(" - A second person supports the arm: calibration DISABLES MOTOR TORQUE.")
    print(" - Mechanical has confirmed safe joint ranges and cable clearance.")
    print(" - Fused power and a reachable PHYSICAL emergency stop are available.")
    print(" - You will move joints gently by hand; no hard-stop detection exists.")
    print(" - Wrist roll's 0–4095 setting is NOT a validated safe limit.")
    try:
        approval = input("\nType CALIBRATE to start, or Enter to cancel: ").strip()
    except (EOFError, KeyboardInterrupt):
        print("\nCancelled.")
        return 1
    if approval != "CALIBRATE":
        print("Cancelled. No connection made.")
        return 0

    print("\nLaunching interactive calibration. Follow the terminal prompts.\n")
    try:
        return subprocess.call(command)
    except FileNotFoundError as exc:
        print(f"Unable to start calibration: {exc}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print("\nInterrupted. Verify arm state and support before proceeding.")
        return 130


if __name__ == "__main__":
    raise SystemExit(main())
