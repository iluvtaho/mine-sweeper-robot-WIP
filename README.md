# mine-sweeper-robot

**Mine sweeper robot built to strengthen and apply understanding of concepts (Electrical and Control Systems, Autonomous Robotics, and Mechanical Design) learned during self-directed study of MITOpenCourseWare's Undergraduate Course: "2.12 Introduction to Robotics"**

## Overview
<img src="assets/videos/Demo GIF.GIF" alt="Demo GIF here" height="300" width="300"/>

### Course Specification & Objectives

> **Goal and Rules:**  
> *"The goal of this project to produce a robot capable of searching for hidden metal disks (“mines”) in a open space. Students design and implement a search algorithm that lets the robot hunt autonomously, and when a mine is found to pause on top of it. Identify as many mines as possible within a given time, say 2 minutes. Every time a fake mine is found, the robot must stop at that position and turn the buzzer on. If the mine is a newly found one, a credit point is given to the project team. If the mine has already been found by the robot, no credit is given. The robot resumes searching a mine."*
>
> — **Source:** *MIT OpenCourseWare: 2.12 Introduction to Robotics (Lab 3)*

---

### Testing Environment Adaptations
To adapt the original MIT OCW specification to locally available resources and workspace constraints, the experimental setup was modified as follows:

* **Original Specification:** 10 metallic mines in a 12' x 12' arena enclosed with 6" cushioned walls and rounded corners.
* **Modified Setup:** 5 metallic mines in a 90 cm x 210 cm enclosed minefield bounded by physical walls and sharp corners. 

<img width="493" height="300" alt="image" src="https://github.com/user-attachments/assets/633eb7ce-4da2-48f5-91c0-7175061764d0" /> (Insert photo of used minefield)

## System Overview
**Primary Function:** Executes an autonomous side-to-side sweep pattern, detects metallic/conductive simulated hazards via an inductive proximity switch, triggers audible alarms, and resumes sweep execution. <br>
**Obstacle Avoidance:** Infrared Obstacle Avoidance sensors located on each side of the front to detect physical barriers and signal the robot to reposition automatically <br>
**Control Method:** Use of NodeMCU-32S to run a fully autonomous closed loop finite state machine without outside control/influence

> *Design note: The robot drives in a side-to-side motion to maximize sweep area and to compensate for the inductive proximity switches small (8 mm) sensing radius.*

### State Machine 

<img width="1567" height="667" alt="image" src="https://github.com/user-attachments/assets/dab829b8-b491-4a50-83df-1718f96e5d18" />

## Bill of Materials

| Component | Model / Type | Function |
| :--- | :--- | :--- |
| **Microcontroller** | NodeMCU-32S | Executes state machine, motor timing, and sensor interrupts |
| **Mine Detection Sensor** | Inductive Proximity Switch LJ18A3-8-Z/BX – NPN NO 8mm Detection | Non-contact detection of metallic target zones / simulated mines |
| **Obstacle Avoidance** | IR Obstacle Avoidance Sensor | Detects obstacles within 6 cm |
| **Alert System** | Active Piezo Buzzer | Audible indicator during mine detection |
| **Motor Driver** | L298N Dual H-Bridge Motor Driver | H-Bridge dual DC motor drive control |
| **Drive Motors** | TT DC Motor 6v | Drive motors |
| **Power** | ~~4x AA Alkaline Batteries (6v), 9v Alkaline Battery~~ 2S 3.7v 3800 mAh Lithium Ion 18650 batteries| The 18650s power all the electronic components|

## Code Structure

[*View Final C++ Code*](./assets/code/Metal_Detecting_Robot.ino)

## Trials and Iterations

To optimize the search algorithm and minimize the time required to clear all 5 targets, the system underwent five trials. 

> *Note: Due to hardware constraints (8mm detection radius and compact chassis size), the physical minefield scale was adapted from the original MIT OCW specifications. Trial videos are accelerated 5x due to GitHub repository file size constraints. [Unedited real-time footage is available here](https://drive.google.com/drive/folders/1LTC-wA61IuTCIbSAw-TUQX8wArLBp5uN?usp=sharing).*

### 1st Trial:

[*First Trial Video*](./assets/videos/First%20Trial%20(5x%20speed).mp4)

* **Performance:** The robot successfully cleared the minefield in **4 minutes and 34 seconds**.
* **Trial Observation:** While baseline obstacle avoidance worked, the robot occasionally became trapped in "avoidance loops" in corners. Furthermore, the wheels physically snagged on the raised edges of the metallic targets, inadvertently pulling the robot directly over the mine. These problems caused lots of time to be wasted, especially the loop traps.
* **Root Causes:** 
  1. The alternating `millis()` timer logic for turning randomly selected directions, leading to repetitive corner trapping. 
  2. Target disks were not mounted flush with the arena floor, causing mechanical interference.
* **Iterations:** 
  * **Software:** Implemented a deterministic escape sequence in the state machine, upon reversing from an obstacle, the robot now forces a turn in the opposite direction of its previous approach. Pivot delay times were kept tight to prevent rear-collisions. Lowered the PWM value to the piezo buzzer to reduce acoustic harshness during alerts.
  * **Hardware:** Secured the metallic targets to the arena floor to ensure sensor triggers were based purely on inductive detection, not mechanical snagging.
* **Outcome:** The robot no longer gets caught in avoidance loops and can easily traverse the minefield without getting caught on the mines.

### 2nd Trial:

[*Second Trial Video*](./assets/videos/Second%20Trial%20Video%20(5x%20speed).mp4)

* **Performance:** The robot unsuccessfully cleared the minefield. Trial took **6 minutes and 30 seconds**
* **Trial Observation:** During the escape sequence (accelerating from a dead stop after detecting a mine), the robot frequently stuttered, spun endlessly in circles, or completely reset its state machine, registering the same mine multiple times.
* **Root Causes:**
  1. Starting two DC gearmotors simultaneously from a dead stop draws a massive spike of "stall current" (estimated >3.0 Amps). The 4S AA alkaline power supply could not deliver this burst current, causing the system voltage to sag momentarily below the NodeMCU-32S's minimum operating threshold (3.3V). This crashed the microcontroller, wiping the debounce memory and forcing a reboot.
* **Iterations:** 
  * **Software:** Attempted to bypass this hardware limitation using a software-defined power management technique known as "load sequencing" (peak shaving). I rewrote the movement and escape patterns to stagger the motor starts, powering only one motor at a time in a "waddle" gait. I also reduced the peak PWM duty cycles and increased the millisecond delays between movements to give the batteries recovery time between surges.
  * **Outcome:** While the software mitigation slightly reduced the frequency of the reboots, the high internal resistance of standard AA alkaline batteries proved too extreme. The voltage sag still occasionally crashed the ESP32 under physical load, indicating that a software-only fix was insufficient and a hardware power redesign was required.

### 3rd Trial:

[*Third Trial Video*](./assets/videos/Third%20Trial%20Video%20(5x%20speed).mp4)

* **Performance:** The robot unsuccessfully cleared the minefield. Trial took **2 minutes and 21 seconds**
* **Trial Observation:** After upgrading two 2 18650 Li-Ion batteries, the robot worked perfectly fine for the first 1 minute. The IR sensors needed some recalibration as their detection distance was too far and would cause the robot to avoid mines that were close to the wall. After the 2 minute mark the robot seemingly lost power and froze, therefore prompting me to stop the trial. 
* **Root Causes:**
  1. Although the Li-ion batteries are perfectly capable of powering the robot, in certain situations where the motors draw lots of current (bumping into a wall, immediate backup when detecting an obstacle, etc.), their voltage drops significantly. The batteries are perfectly capable of handling these current spikes but the frequency in which they occur cause them to drain very quickly.
* **Iterations:** 
  * **Software:** I lowered the time taken when reversing to extend the lifetime of the robot. I also adjusted the direction change interval and motor PWM values to accommodate for the new speed provided by the Li-ion batteries.
  * **Hardware:** Removed the 9v battery as it ran out of juice (I left it connected for 3 days) and powered the inductive proximity switch with the Li-Ion. battery instead
* **Outcome:** Speed adjustment proved to be successful and robot can travel in a straight sweeping line. However, the attempt at extending the lifetime was unsuccessful and it could only be active for ~4 minutes, which is not enough time to detect all the mines.

### 4th Trial:

[*Fourth Trial Video*](./assets/videos/Fourth%20Trial%20Video%20(5x%20speed).mp4)

* **Performance:** The robot successfully cleared the minefield. Trial took **1 minute and 53 seconds**
* **Trial Observation:** The most successful trial so far. The robot didn't get caught in any "avoidance loops", lose power, or need any human interference to keep going. The only problem, however, is how the sweep isn't wide enough and it would still not detect mines despite nearly driving directly over them.
* **Root Causes:**
  1. The turn direction interval is too short to do a full sweep.
* **Iterations:** 
  * **Software:** Extended the interval from 600 ms to 800 ms, while also lowering the motor PWM values to compensate for the longer active time.
* **Outcome:** In the uncut video, I didn't notice that it had detected all the mines and left it running for about 6 minutes. This caused the Li-ion batteries to expend most of their capacity. Whenever I tried to test the robot again it would only run for about 1 minute before freezing. Since the batteries are rechargeable I'll be getting a charging module to refill the batteries soon and get on with the final trial. 
