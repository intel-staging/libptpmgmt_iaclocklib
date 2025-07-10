---
title: PROXY_CFG
section: 5
date: July 2025
version: 1.4
manual: File Formats Manual
---

<!-- SPDX-License-Identifier: GFDL-1.3-no-invariants-or-later
SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. -->

NAME
====
proxy_cfg - Clock Manager proxy daemon configuration file  

DESCRIPTION
===========
`clkmgr_proxy`(8) reads configuration data from /etc/clkmgr/proxy_cfg.json  
(or the file specified with -f on the command line). This configuration file  
is written in JSON format and defines how the clock manager proxy daemon should  
initialize and manage time synchronization services on the system.  

The file contains a set of keywords, each representing a specific configuration  
option or service. These keywords allow users to customize the behavior of  
`clkmgr_proxy`, including which time bases are available, how each time base  
is named, and how services like `ptp4l` and `chrony` are configured for each  
time base.  

PARAMETERS
==========

`timeBases`

    Array of configuration objects for each available time base.

`ptp4l`

    Object specifying configuration for the `ptp4l` service.

    `interfaceName`
    Network interface name.

    `udsAddr`
    Unix domain socket address.

    `domainNumber`
    PTP domain number.

    `transportSpecific`
    Transport-specific field.

    If omitted, no `ptp4l` service is configured.  
    If present but empty, default values are assigned.

`chrony`

    Object specifying configuration for the `chrony` service.

    `udsAddr`
    Unix domain socket address.

    If omitted, no `chrony` service is configured.  
    If present but empty, default values are assigned.

`timeBaseName`

    The `timeBaseName` parameter specified a unique name to each time base to  
    distinguish between different time bases in the configuration.  
    The `timeBaseName` must be specified within length [64 characters].  

`interfaceName`

    The `interfaceName` parameter specified the network interface name used by  
    the time base for the `ptp4l` service.  
    The `interfaceName` must be specified within length [64 characters].  

`udsAddr`

    The `udsAddr` parameter specified the Unix domain socket address used for  
    inter-process communication.  
    Both `ptp4l` and `chrony` service have their own `udsAddr` fields to indicate  
    the socket path for their respective services.  
    The `udsAddr` must be specified within the length [64 characters].  

    To specify/check the `udsAddr` for `ptp4l` service, go to `ptp4l`  
    configuration file and specify/check for the `uds_address` parameter as below:  

        uds_address /var/run/ptp4l-domain-0  

    To specify/check the `udsAddr` for `chrony` service, go to `libchrony`  
    `socket.c` in the function `chrony_open_socket` specify/check for the  
    `open_unix_socket` parameter as below:  

        fd = open_unix_socket("/var/run/chrony/chronyd.sock");  

`domainNumber`

    The PTP domain number in the range [0 .. 255].  
    To specify/check the PTP domain number for the `ptp4l` service, go to  
    `ptp4l` configuration file and specify/check for the `domainNumber` parameter  
    as below:  

        domainNumber 0  

`transportSpecific`

    The transport-specific field value for PTP. In the JSON configuration file,  
    the value `transportSpecific` must be specified using ONLY decimal notation  
    in the range [0 .. 255].  
    While in the `ptp4l` configuration file, this parameter can be specified  
    using either hexadecimal (e.g., `0x1`) or decimal (e.g., `1`) notation as  
    below:  

        transportSpecific 0x1  

                OR  

        transportSpecific 1  

EXAMPLES
========

Below are examples of JSON configuration files that users can create refer for  
different scenarios.  

1. Single ptp4l instance  

    In this scenario, the system has only one `ptp4l` instance configured under  
    a single time base. No `chrony` service is included. The clock synchronized  
    by `ptp4l` serves as the reference clock for the system. Users are expected  
    to subscribe to the synchronization results from the `ptp4l` service. The  
    JSON configuration includes only the `ptp4l` object within the time base.  

    **Example:**  

    ```json
    {
        "timeBases": [
            {
                "timeBaseName": "Global Clock",
                "ptp4l":
                {
                    "interfaceName": "eth0",
                    "udsAddr": "/var/run/ptp4l-domain-0",
                    "domainNumber": 0,
                    "transportSpecific": 1
                }
            }
        ]
    }
    ```

2. Single chrony instance  

    In this scenario, the system has only one `chrony` instance configured under  
    a single time base. No `ptp4l` service is included. The clock synchronized  
    by `chrony` serves as the reference clock for the system. Users are expected  
    to subscribe to the synchronization results from the `chrony` service. The  
    JSON configuration includes only the `chrony` object within the time base.  

    **Example:**  

    ```json
    {
        "timeBases": [
            {
                "timeBaseName": "Global Clock",
                "chrony":
                {
                    "udsAddr": "/var/run/chrony/chronyd.sock"
                }
            }
        ]
    }
    ```

3. Single ptp4l + Single chrony instance (default value)  

    In this scenario, the system has one `ptp4l` instance and one `chrony`  
    instance. The clock synchronized by `ptp4l` serves as the reference clock  
    for `chrony`, so both are grouped under a single time base. Users are  
    expected to subscribe to the synchronization results from both `ptp4l` and  
    `chrony`. Therefore, both services are configured within the same time base  
    in the JSON configuration.  
    If any fields in the `ptp4l` or `chrony` objects are not specified, default  
    values will be assigned as follows:  

    **Default values:**
    - **`ptp4l`**  

        - `interfaceName`: ""  
        - `udsAddr`: "/var/run/ptp4l"  
        - `domainNumber`: 0  
        - `transportSpecific`: 1  

    - **`chrony`**  

        - `udsAddr`: "/var/run/chrony/chronyd.sock"

    **Example:**  

    ```json
    {
        "timeBases": [
            {
                "timeBaseName": "Global Clock",
                "ptp4l": {},
                "chrony": {}
            }
        ]
    }
    ```

4. Single ptp4l + Single chrony instance (with assigned values)  

    In this scenario, the system has one `ptp4l` instance and one `chrony`  
    instance. The clock synchronized by `ptp4l` serves as the reference clock  
    for `chrony`, so both are grouped under a single time base. Users are  
    expected to subscribe to the synchronization results from both `ptp4l` and  
    `chrony`. Therefore, both services are configured within the same time base  
    in the JSON configuration.  

    **Example:**  

    ```json
    {
        "timeBases": [
            {
                "timeBaseName": "Global Clock",
                "ptp4l":
                {
                    "interfaceName": "eth0",
                    "udsAddr": "/var/run/ptp4l",
                    "domainNumber": 0,
                    "transportSpecific": 1
                },
                "chrony":
                {
                    "udsAddr": "/var/run/chrony/chronyd.sock"
                }
            }
        ]
    }
    ```

5. Multiple ptp4l + Single chrony instances  

    In this scenario, the system has multiple `ptp4l` instances and one `chrony`  
    instance. Each `ptp4l` instance is configured under its own time base, while  
    the `chrony` instance is grouped with one of the time bases (typically the  
    reference clock). Users are expected to subscribe to the synchronization  
    results from both `ptp4l` and `chrony`. Therefore, the JSON configuration  
    includes multiple time bases, each with its own `ptp4l` configuration, and  
    one of them also includes the `chrony` configuration.  

    **Example:**  

    ```json
    {
        "timeBases": [
            {
                "timeBaseName": "Global Clock",
                "ptp4l":
                {
                    "interfaceName": "eth0",
                    "udsAddr": "/var/run/ptp4l",
                    "domainNumber": 0,
                    "transportSpecific": 1
                },
                "chrony":
                {
                    "udsAddr": "/var/run/chrony/chronyd.sock"
                }
            },
            {
                "timeBaseName": "Working Clock",
                "ptp4l":
                {
                    "interfaceName": "eth1",
                    "udsAddr": "/var/run/ptp4l-domain-20",
                    "domainNumber": 20,
                    "transportSpecific": 1
                }
            }
        ]
    }
    ```

SEE ALSO
========
`clkmgr_proxy`(8)

<!-- ## Converting to Man Page
> To generate a `.8` man page from this Markdown file, use the following command:
    ```sh
    pandoc -s -f markdown -t man clkmgr_proxy_parser.md -o clkmgr_proxy_parser.8
    ```
-->
