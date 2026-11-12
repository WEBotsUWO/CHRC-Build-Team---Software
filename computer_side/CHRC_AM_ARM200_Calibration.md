# CHRC AM-ARM200 — Calibration Quick Guide

**Purpose:** Calibrate the **six-joint CHRC follower arm** using the [AM-ARM200 LeRobot fork](https://github.com/liyiteng/lerobot_alohamini). No leader arm is needed.

## Before starting

1. Install the fork using its [installation guide](https://github.com/liyiteng/lerobot_alohamini/blob/main/docs/alohamini/install.md) in an activated Python environment. Connect the follower arm and find its serial port with `lerobot-find-port` (e.g., `COM5` on Windows).
2. **TODO — Software:** In `src/lerobot/robots/so_follower/so_follower.py`, add an `_ARM_PROFILES` entry named `am-chrc-6dof`, copied from `am-follower-6dof` **but without** `("gripper", 7, ...)`. Verify servo IDs 1–6 and motor types match the assembled CHRC arm. The repository's stock profile expects motor 7.
3. **TODO — Mechanical:** Confirm each joint's *safe physical travel*, including collision and cable limits. Have someone securely support the arm: **calibration disables motor torque, so joints may fall under gravity**. Use the team's fused supply and accessible **physical emergency stop**. Do not force joints against stops.

## Start calibration

From the activated environment, run:

```bash
python start_chrc_calibration.py --port COM5
```

Replace `COM5` with **your** port (`/dev/ttyACM0`, etc.). Use `--dry-run` to preview without touching hardware.

The script launches LeRobot's official `lerobot_calibrate` module with robot type `so101_follower`, robot ID `chrc_follower`, and the **custom six-motor** profile.

## What happens during calibration

1. **Reference:** The driver disables torque. Manually move the joints to an approximately central, safe pose; press **Enter**. The driver records homing offsets.
2. **Range:** Move each joint **slowly and separately** through its previously verified safe travel; press **Enter** when finished. LeRobot continually records the **minimum and maximum encoder readings it observes**.
3. **Save:** The driver writes calibration to the servos and saves a calibration file. Note its printed location; power-cycle the arm according to the upstream guide, then verify reported positions before commanding any movements.

**Critical:** The driver **does not sense a hard mechanical stop or decide when a limit has been reached**. It records the furthest readings **you** demonstrate. Do not use calibration as a collision-finding procedure. In this fork, `wrist_roll` is assigned the full **0–4095 encoder range** rather than manually swept: **this is not a verified safe wrist limit**. Apply separate, mechanically validated software limits.

## Record for the DOF handoff

| ID | Joint | Safe min (°) | Safe max (°) | Calibrated encoder min/max |
|---|---|---|---|---|
| 1 | shoulder_pan | **TODO** | **TODO** | **TODO** |
| 2 | shoulder_lift | **TODO** | **TODO** | **TODO** |
| 3 | elbow_flex | **TODO** | **TODO** | **TODO** |
| 4 | wrist_flex | **TODO** | **TODO** | **TODO** |
| 5 | wrist_yaw | **TODO** | **TODO** | **TODO** |
| 6 | wrist_roll | **TODO** | **TODO** | **0–4095 assumed by driver; verify safe range** |

**Source:** [Follower calibration implementation](https://github.com/liyiteng/lerobot_alohamini/blob/main/src/lerobot/robots/so_follower/so_follower.py) and [AM-ARM200 workflow](https://github.com/liyiteng/lerobot_alohamini/blob/main/docs/alohamini/am-arm200.md). These are **driver-relative** calibrated measurements, not automatically CAD-defined joint angles.
