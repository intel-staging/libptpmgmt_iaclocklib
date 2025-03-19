<!-- SPDX-License-Identifier: GFDL-1.3-no-invariants-or-later
     SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. -->
# The diagram of Clock Manager usage in system: (This need to update as well)

Test app <----> client runtime(libclkmgr.so) <----> clkmgr_proxy <----> libptpmgmt.so <----> ptp4l

# How to get Linux PTP:

You can install it from your Linux distribution:  
On Ubuntu and Debian:  
```bash
sudo apt install linuxptp
```
On Fedora:  
```bash
dnf install linuxptp
```

Or get it from [linuxptp](https://linuxptp.nwtime.org/) site.

# How to get Chrony and LibChrony

Chrony is divided to two parts:  
  * Chronyd: The Chronyd application  
  * LibChrony: The Chrony library  

1. Install Chronyd (Some system might have Chronyd pre-installed):  
    ```bash
    git clone https://gitlab.com/chrony/chrony.git
    ```

2. Install LibChrony:  
    ```bash
    git clone https://gitlab.com/chrony/libchrony.gitcd libchrony
    cd libchrony
    make
    make install prefix=/usr/local
    ldconfig
    ```

# How to Clone and Build the Clock Manager together with libptpmgmt:

Clock manager is divided to two parts:  
  * clkmgr_proxy: The Clock manager proxy.  
  * libclkmgr.so: The Clock manager user library.  

We provide libclkmgr.so with API for user applications to connect, subscribe and
receive ptp4l events forward by the Clock manager proxy.  
The Clock manager proxy uses libptpmgmt.so to subscribe and to listen for events received from ptp4l.  
libclkmgr.so communicates with the Clock manager proxy through a message queue.

1. Install package dependencies:  
    ```bash
    sudo apt install swig libtool-bin cppcheck doxygen ctags astyle dot epstopdf valgrind
    ```

2. Install Real-Time Priority Inheritance Library (librtpi):  
    On new Ubuntu and Debian systems:  
    ```bash
    sudo apt install librtpi-dev
    ```
    On older systems, build and install the library:  
    ```bash
    git clone https://github.com/linux-rt/librtpi.git  
    cd librtpi  
    autoreconf --install  
    ./configure  
    make  
    make install
    ```
3. Clone the repository:
    ```bash
    git clone https://github.com/erezgeva/libptpmgmt
    ```

4. Navigate to the cloned directory:
    ```bash
    cd libptpmgmt
    ```

5. Build the application:
    ```bash
    autoreconf -i
    ./configure
    make
    ```

6. Outcome: two libraries and one application
    ```bash
    .libs/libptpmgmt.so
    .libs/libclkmgr.so
    clkmgr/proxy/clkmgr_proxy
    ```

# How to Build the Sample Application:

We provided a sample code of testing applications which will uses
Clock Manager API to track latest status of ptp4l.  
The applications are provided for demonstration only.  
We do not recommand to use them for production.  

1. Navigate to the sample directory:
    ```bash
    cd libptpmgmt/clkmgr/sample
    ```

2. Build the application:
    ```bash
    make
    ```

3. Outcome: two binary
    ```bash
    clkmgr_test
    clkmgr_c_test
    ```

# How to test:

In current state, total of two types of use case are supported in Clock Manager:
1. Single Domain
2. Multi Domain
The test steps and results of each use cases will be shown in below:

# Test Steps:

1. For Multi Domain user need to create vclock in order to support multi domain:  
    (Single Domain may skip this step to step 2)
    Create vclock (Support up to 4 vclock?):
    ```bash
    echo <number of vclock> > /sys/class/net/enp1s0/device/ptp/ptp0/n_vclocks
    ```

2. Run the ptp4l service on both DUT and link partner:  
    Before start ptp4l service, user need to prepare the configuration file  
    accordingly. (Probably prepare a sample?)
    ***Single Domain***
    If linuxptp is pre-installed on system:
    ```bash
    sudo ptp4l -i <interface name> -f <config file> -m
    ```
    IF you installed it yourself
    ```bash
    cd <folder>
    sudo ./ptp4l -i <interface name> -f <config file> -m
    ```
    ***Multi Domain***
    Note:  
    For multi domain, ptp4l need to run the CMLDS service.  
    Run the ptp4l CMLDS service on both DUT and link partner:
    ```bash
    sudo ptp4l -i <interface name> -f <cmlds config file> -m
    ```
    Run the ptp4l domains service on both DUT and link partner:
    ```bash
    sudo ptp4l -i <interface name> -f <domain config file> -m
    ```

3. Add ptp device (e.g /dev/ptp0) as refclock for chrony daemon application on DUT:
    ```bash
    echo "refclock PHC /dev/ptp0 poll -6 dpoll -1" >>  /etc/chrony/chrony.conf
    ```
4. Run the chrony daemon application on DUT:  
    If chronyd is pre-installed on system:
    ```bash
    chronyd -f /etc/chrony/chrony.conf
    ```
    If you installed it yourself
    ```bash
    cd <folder>
    sudo ./chronyd -f /etc/chrony/chrony.conf
    ```

5. Run the clkmgr_proxy application on DUT:  
    For how to prepare the proxy configuration file, can refer to [sample] (/libptpmgmt/clkmgr/proxy/proxy_cfg.json/)
    ```bash
    cd libptpmgmt/clkmgr/proxy
    sudo ./run_proxy.sh
    ```
6. Run the sample application on DUT:
    ***Single Domain***
    No additional arguments needed to support single domain mode.

    ***Multi Domain***
    There are 3 modes in multi domain:
    1. Default Mode: In default mode, the sample application will direct  
                     subscribe to time base index 1.  
    2. Subscribe All Mode (-a): In this mode, the sample application will  
                                subscribe to all available time bases.  
    3. User Prompt Mode (-p): In this mode, the sample application will prompt  
                              user to subscribe up to multiple time bases.  
    a. c++ sample application:
    ```bash
    cd libptpmgmt/clkmgr/client
    sudo ./run_clkmgr_test.sh <optional arguments>
    ```

    b. c sample application:
    ```bash
    cd libptpmgmt/clkmgr/client
    sudo ./run_clkmgr_c_test.sh <optional arguments>
    ```

# Examples of result:

Usage of proxy daemon (clkmgr_proxy):
```bash
~/libptpmgmt/clkmgr/proxy# ./run_proxy.sh -h
Usage of ./clkmgr_proxy:
Options:
 -f [file] Read configuration from 'file'
 -l <lvl> Set log level
          0: ERROR, 1: INFO(default), 2: DEBUG, 3: TRACE
 -v <0|1> Enable or disable verbose output
          0: disable, 1: enable(default)
 -s <0|1> Enable or disable system log printing
          0: disable, 1: enable(default)
 -h       Show this help message
```

Usage of c++ sample application (clkmgr_test):
```bash
~/libptpmgmt/clkmgr/sample# ./run_clkmgr_test.sh -h
Usage of ./clkmgr_test :
Options:
  -a subscribe to all time base indices
     Default: timeBaseIndex: 1
  -p enable user to subscribe to specific time base indices
  -s subscribe_event_mask
     Default: 0xf
     Bit 0: eventGMOffset
     Bit 1: eventSyncedToGM
     Bit 2: eventASCapable
     Bit 3: eventGMChanged
  -c composite_event_mask
     Default: 0x7
     Bit 0: eventGMOffset
     Bit 1: eventSyncedToGM
     Bit 2: eventASCapable
  -u gm offset upper limit (ns)
     Default: 100000 ns
  -l gm offset lower limitt (ns)
     Default: -100000 ns
  -i idle time (s)
     Default: 1 s
  -m chrony offset upper limit (ns)
     Default: 100000 ns
  -n chrony offset lower limit (ns)
     Default: -100000 ns
  -t timeout in waiting notification event (s)
     Default: 10 s
```

***Single Domain***
Example output of c++ sample application (clkmgr_test):
```bash
~/libptpmgmt/clkmgr/sample# ./run_clkmgr_test.sh -l 0 -u 100 -t 0
[clkmgr] set subscribe event : 0xf
[clkmgr] set composite event : 0x7
GM Offset upper limit: 100 ns
GM Offset lower limit: 0 ns
Chrony Offset upper limit: 100000 ns
Chrony Offset lower limit: -100000 ns

[clkmgr] List of available clock:
TimeBaseIndex: 1
timeBaseName: Global Clock
udsAddrChrony: /var/run/chrony/chronyd.sock
udsAddrPtp4l: /var/run/ptp4l-domain-0
interfaceName: eth0
transportSpecific: 1
domainNumber: 0

TimeBaseIndex: 2
timeBaseName: Working Clock
udsAddrChrony:
udsAddrPtp4l: /var/run/ptp4l-domain-20
interfaceName: eth1
transportSpecific: 1
domainNumber: 20

Subscribe to time base index: 1
[clkmgr][673242.033] Obtained data from Subscription Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406256034734074 ns
+---------------------------+------------------------+
| Event                     | Event Status           |
+---------------------------+------------------------+
| offset_in_range           | 1                      |
| synced_to_primary_clock   | 0                      |
| as_capable                | 0                      |
| gm_Changed                | 1                      |
+---------------------------+------------------------+
| GM UUID                   | 000001.0000.000000     |
| clock_offset              | 2                   ns |
| notification_timestamp    | 1742406256034696341 ns |
+---------------------------+------------------------+
| composite_event           | 0                      |
| - offset_in_range         |                        |
| - synced_to_primary_clock |                        |
| - as_capable              |                        |
+---------------------------+------------------------+

+---------------------------+------------------------+
| chrony offset_in_range    | 1                      |
+---------------------------+------------------------+
| chrony clock_offset       | 23                  ns |
| chrony clock_reference_id | 50484330               |
| chrony polling interval   | 500000              us |
+---------------------------+------------------------+

[clkmgr][673243.033] Waiting Notification from time base index 1 ...
[clkmgr][673243.033] Obtained data from Notification Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406257034923918 ns
+---------------------------+--------------+-------------+
| Event                     | Event Status | Event Count |
+---------------------------+--------------+-------------+
| offset_in_range           | 1            | 2           |
| synced_to_primary_clock   | 0            | 0           |
| as_capable                | 0            | 0           |
| gm_Changed                | 0            | 0           |
+---------------------------+--------------+-------------+
| GM UUID                   |     000001.0000.000000     |
| clock_offset              |     3                   ns |
| notification_timestamp    |     1742406256961242310 ns |
+---------------------------+--------------+-------------+
| composite_event           | 0            | 0           |
| - offset_in_range         |              |             |
| - synced_to_primary_clock |              |             |
| - as_capable              |              |             |
+---------------------------+--------------+-------------+

+---------------------------+----------------------------+
| chrony offset_in_range    | 1            | 0           |
+---------------------------+----------------------------+
| chrony clock_offset       |     23                  ns |
| chrony clock_reference_id |     50484330               |
| chrony polling_interval   |     500000              us |
+---------------------------+----------------------------+
```

***Multi Domain***
Example output of c++ sample application (clkmgr_test):
1. Default Mode:
```bash
~/libptpmgmt/clkmgr/sample# ./run_clkmgr_test.sh -l 0 -u 100 -t 0
[clkmgr] set subscribe event : 0xf
[clkmgr] set composite event : 0x7
GM Offset upper limit: 100 ns
GM Offset lower limit: 0 ns
Chrony Offset upper limit: 100000 ns
Chrony Offset lower limit: -100000 ns

[clkmgr] List of available clock:
TimeBaseIndex: 1
timeBaseName: Global Clock
udsAddrChrony: /var/run/chrony/chronyd.sock
udsAddrPtp4l: /var/run/ptp4l-domain-0
interfaceName: eth0
transportSpecific: 1
domainNumber: 0

TimeBaseIndex: 2
timeBaseName: Working Clock
udsAddrChrony:
udsAddrPtp4l: /var/run/ptp4l-domain-20
interfaceName: eth1
transportSpecific: 1
domainNumber: 20

Subscribe to time base index: 1
[clkmgr][673242.033] Obtained data from Subscription Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406256034734074 ns
+---------------------------+------------------------+
| Event                     | Event Status           |
+---------------------------+------------------------+
| offset_in_range           | 1                      |
| synced_to_primary_clock   | 0                      |
| as_capable                | 0                      |
| gm_Changed                | 1                      |
+---------------------------+------------------------+
| GM UUID                   | 000001.0000.000000     |
| clock_offset              | 2                   ns |
| notification_timestamp    | 1742406256034696341 ns |
+---------------------------+------------------------+
| composite_event           | 0                      |
| - offset_in_range         |                        |
| - synced_to_primary_clock |                        |
| - as_capable              |                        |
+---------------------------+------------------------+

+---------------------------+------------------------+
| chrony offset_in_range    | 1                      |
+---------------------------+------------------------+
| chrony clock_offset       | 23                  ns |
| chrony clock_reference_id | 50484330               |
| chrony polling interval   | 500000              us |
+---------------------------+------------------------+

[clkmgr][673243.033] Waiting Notification from time base index 1 ...
[clkmgr][673243.033] Obtained data from Notification Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406257034923918 ns
+---------------------------+--------------+-------------+
| Event                     | Event Status | Event Count |
+---------------------------+--------------+-------------+
| offset_in_range           | 1            | 2           |
| synced_to_primary_clock   | 0            | 0           |
| as_capable                | 0            | 0           |
| gm_Changed                | 0            | 0           |
+---------------------------+--------------+-------------+
| GM UUID                   |     000001.0000.000000     |
| clock_offset              |     3                   ns |
| notification_timestamp    |     1742406256961242310 ns |
+---------------------------+--------------+-------------+
| composite_event           | 0            | 0           |
| - offset_in_range         |              |             |
| - synced_to_primary_clock |              |             |
| - as_capable              |              |             |
+---------------------------+--------------+-------------+

+---------------------------+----------------------------+
| chrony offset_in_range    | 1            | 0           |
+---------------------------+----------------------------+
| chrony clock_offset       |     23                  ns |
| chrony clock_reference_id |     50484330               |
| chrony polling_interval   |     500000              us |
+---------------------------+----------------------------+
```

2. Subscribe All Mode (-a):
```bash
~/libptpmgmt/clkmgr/sample# ./run_clkmgr_test.sh -l 0 -u 100 -t 0 -a
[clkmgr] set subscribe event : 0xf
[clkmgr] set composite event : 0x7
GM Offset upper limit: 100 ns
GM Offset lower limit: 0 ns
Chrony Offset upper limit: 100000 ns
Chrony Offset lower limit: -100000 ns

[clkmgr] List of available clock:
TimeBaseIndex: 1
timeBaseName: Global Clock
udsAddrChrony: /var/run/chrony/chronyd.sock
udsAddrPtp4l: /var/run/ptp4l-domain-0
interfaceName: eth0
transportSpecific: 1
domainNumber: 0

TimeBaseIndex: 2
timeBaseName: Working Clock
udsAddrChrony:
udsAddrPtp4l: /var/run/ptp4l-domain-20
interfaceName: eth1
transportSpecific: 1
domainNumber: 20

Subscribe to time base index: 1
[clkmgr][673368.157] Obtained data from Subscription Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406382159517446 ns
+---------------------------+------------------------+
| Event                     | Event Status           |
+---------------------------+------------------------+
| offset_in_range           | 1                      |
| synced_to_primary_clock   | 0                      |
| as_capable                | 0                      |
| gm_Changed                | 1                      |
+---------------------------+------------------------+
| GM UUID                   | 000001.0000.000000     |
| clock_offset              | 1                   ns |
| notification_timestamp    | 1742406382159481625 ns |
+---------------------------+------------------------+
| composite_event           | 0                      |
| - offset_in_range         |                        |
| - synced_to_primary_clock |                        |
| - as_capable              |                        |
+---------------------------+------------------------+

+---------------------------+------------------------+
| chrony offset_in_range    | 1                      |
+---------------------------+------------------------+
| chrony clock_offset       | -24                 ns |
| chrony clock_reference_id | 50484330               |
| chrony polling interval   | 500000              us |
+---------------------------+------------------------+

Subscribe to time base index: 2
[clkmgr][673368.157] Obtained data from Subscription Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406382159619992 ns
+---------------------------+------------------------+
| Event                     | Event Status           |
+---------------------------+------------------------+
| offset_in_range           | 1                      |
| synced_to_primary_clock   | 0                      |
| as_capable                | 0                      |
| gm_Changed                | 1                      |
+---------------------------+------------------------+
| GM UUID                   | 000002.0000.000000     |
| clock_offset              | 1                   ns |
| notification_timestamp    | 1742406382159601660 ns |
+---------------------------+------------------------+
| composite_event           | 0                      |
| - offset_in_range         |                        |
| - synced_to_primary_clock |                        |
| - as_capable              |                        |
+---------------------------+------------------------+

+---------------------------+------------------------+
| chrony offset_in_range    | 0                      |
+---------------------------+------------------------+
| chrony clock_offset       | 0                   ns |
| chrony clock_reference_id | 0                      |
| chrony polling interval   | 0                   us |
+---------------------------+------------------------+

[clkmgr][673369.157] Waiting Notification from time base index 1 ...
[clkmgr][673369.158] Obtained data from Notification Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406383159681353 ns
+---------------------------+--------------+-------------+
| Event                     | Event Status | Event Count |
+---------------------------+--------------+-------------+
| offset_in_range           | 1            | 2           |
| synced_to_primary_clock   | 0            | 0           |
| as_capable                | 0            | 0           |
| gm_Changed                | 0            | 0           |
+---------------------------+--------------+-------------+
| GM UUID                   |     000001.0000.000000     |
| clock_offset              |     1                   ns |
| notification_timestamp    |     1742406383116280455 ns |
+---------------------------+--------------+-------------+
| composite_event           | 0            | 0           |
| - offset_in_range         |              |             |
| - synced_to_primary_clock |              |             |
| - as_capable              |              |             |
+---------------------------+--------------+-------------+

+---------------------------+----------------------------+
| chrony offset_in_range    | 1            | 0           |
+---------------------------+----------------------------+
| chrony clock_offset       |     0                   ns |
| chrony clock_reference_id |     50484330               |
| chrony polling_interval   |     500000              us |
+---------------------------+----------------------------+

[clkmgr][673369.158] sleep for 1 seconds...

[clkmgr][673369.463] Waiting Notification from time base index 2 ...
^C[clkmgr][673369.463] Obtained data from Notification Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406383464841766 ns
+---------------------------+--------------+-------------+
| Event                     | Event Status | Event Count |
+---------------------------+--------------+-------------+
| offset_in_range           | 1            | 4           |
| synced_to_primary_clock   | 0            | 0           |
| as_capable                | 0            | 0           |
| gm_Changed                | 0            | 0           |
+---------------------------+--------------+-------------+
| GM UUID                   |     000002.0000.000000     |
| clock_offset              |     1                   ns |
| notification_timestamp    |     1742406383354370241 ns |
+---------------------------+--------------+-------------+
| composite_event           | 0            | 0           |
| - offset_in_range         |              |             |
| - synced_to_primary_clock |              |             |
| - as_capable              |              |             |
+---------------------------+--------------+-------------+

+---------------------------+----------------------------+
| chrony offset_in_range    | 0            | 0           |
+---------------------------+----------------------------+
| chrony clock_offset       |     0                   ns |
| chrony clock_reference_id |     0                      |
| chrony polling_interval   |     0                   us |
+---------------------------+----------------------------+
```

3. User Prompt Mode (-p):
```bash
~/libptpmgmt/clkmgr/sample# ./run_clkmgr_test.sh -l 0 -u 100 -t 0 -p
[clkmgr] set subscribe event : 0xf
[clkmgr] set composite event : 0x7
GM Offset upper limit: 100 ns
GM Offset lower limit: 0 ns
Chrony Offset upper limit: 100000 ns
Chrony Offset lower limit: -100000 ns

[clkmgr] List of available clock:
TimeBaseIndex: 1
timeBaseName: Global Clock
udsAddrChrony: /var/run/chrony/chronyd.sock
udsAddrPtp4l: /var/run/ptp4l-domain-0
interfaceName: eth0
transportSpecific: 1
domainNumber: 0

TimeBaseIndex: 2
timeBaseName: Working Clock
udsAddrChrony:
udsAddrPtp4l: /var/run/ptp4l-domain-20
interfaceName: eth1
transportSpecific: 1
domainNumber: 20

Enter the time base indices to subscribe (comma-separated, default is 1): 2
Subscribe to time base index: 2
[clkmgr][673423.642] Obtained data from Subscription Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406437644657110 ns
+---------------------------+------------------------+
| Event                     | Event Status           |
+---------------------------+------------------------+
| offset_in_range           | 0                      |
| synced_to_primary_clock   | 0                      |
| as_capable                | 0                      |
| gm_Changed                | 1                      |
+---------------------------+------------------------+
| GM UUID                   | 000002.0000.000000     |
| clock_offset              | 0                   ns |
| notification_timestamp    | 1742406437644621523 ns |
+---------------------------+------------------------+
| composite_event           | 0                      |
| - offset_in_range         |                        |
| - synced_to_primary_clock |                        |
| - as_capable              |                        |
+---------------------------+------------------------+

+---------------------------+------------------------+
| chrony offset_in_range    | 0                      |
+---------------------------+------------------------+
| chrony clock_offset       | 0                   ns |
| chrony clock_reference_id | 0                      |
| chrony polling interval   | 0                   us |
+---------------------------+------------------------+

[clkmgr][673424.643] Waiting Notification from time base index 2 ...
[clkmgr][673424.643] Obtained data from Notification Event:
[clkmgr] Current Time of CLOCK_REALTIME: 1742406438644722913 ns
+---------------------------+--------------+-------------+
| Event                     | Event Status | Event Count |
+---------------------------+--------------+-------------+
| offset_in_range           | 1            | 3           |
| synced_to_primary_clock   | 0            | 0           |
| as_capable                | 0            | 0           |
| gm_Changed                | 0            | 0           |
+---------------------------+--------------+-------------+
| GM UUID                   |     000002.0000.000000     |
| clock_offset              |     0                   ns |
| notification_timestamp    |     1742406438626131885 ns |
+---------------------------+--------------+-------------+
| composite_event           | 0            | 0           |
| - offset_in_range         |              |             |
| - synced_to_primary_clock |              |             |
| - as_capable              |              |             |
+---------------------------+--------------+-------------+

+---------------------------+----------------------------+
| chrony offset_in_range    | 0            | 0           |
+---------------------------+----------------------------+
| chrony clock_offset       |     0                   ns |
| chrony clock_reference_id |     0                      |
| chrony polling_interval   |     0                   us |
+---------------------------+----------------------------+
```

Note:
```
In the absence of a primary clock (GM), the clock offset defaults to 0, which
it's anticipated that the offset_in_range event will be TRUE. Consequently,
the synced_to_primary_clock event is used to ensure that the offset_in_range
event indicates either a high-quality clock synchronization (in-sync) or that
the primary clock is not present (out-of-sync).
```
