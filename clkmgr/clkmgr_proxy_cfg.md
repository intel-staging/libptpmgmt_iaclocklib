<!-- SPDX-License-Identifier: GFDL-1.3-no-invariants-or-later
SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. -->
---
title: PROXY_CFG
section: 5
header: JSON File Formats Manual
date: July 2025
---

# NAME

proxy_cfg - **Clock Manager** proxy daemon JSON configuration file

# DESCRIPTION

**clkmgr_proxy**(8) reads configuration data from this configuration file.
It is written in JSON format and defines how the **Clock Manager**
proxy daemon should initialize and manage the other time synchronization
services on the system.

The file contains a set of keywords, each representing a specific configuration
option or service. These keywords allow users to customize the behavior of
**Clock Manager**, including which time bases are available, how each time base
is named, and how services like **ptp4l**(8) and **chronyd**(8) are configured
for each time base.

# PARAMETERS

## `timeBases`

JSON array of configuration JSON objects for each available time base.

## `timeBaseName`

The `timeBaseName` parameter specified a unique name to each time base to
distinguish between different time bases in the configuration.  
The maximum length is 64 octets. Note that a single UTF-8 character can be
stored with multiple octets.

## `ptp4l`

JSON object specifying configuration for using a **ptp4l**(8) service.
If omitted, **clkmgr_proxy**(8) will not communicate with the **ptp4l**(8)
in this time base. In case of using an empty JSON object,
**clkmgr_proxy**(8) will use its default parameters. Please refer to
**ptp4l**(8) for the UDS address, domain number, and transport specific
used by the **ptp4l**(8) service.

`interfaceName`
: - The name of network interface.  
- The `interfaceName` must be specified within length [64 characters].

`udsAddr`
: - The Unix Domain Socket address used for inter-process communication with
**ptp4l**(8) service.  
- Default parameter: "/var/run/ptp4l"

`domainNumber`
: - The PTP domain number.  
- Range: [0 .. 255]  
- Default parameter: 0

`transportSpecific`
: - Transport specific.  
- Range: [0 .. 255]  
- Default parameter: 1

## `chrony`

JSON object specifying configuration for using a **chronyd**(8) service.
If omitted, **clkmgr_proxy**(8) will not communicate with the **chronyd**(8)
in this time base. In case of using an empty JSON object,
**clkmgr_proxy**(8) will use its default parameters. Please refer to
**chronyd**(8) for the UDS address used by the **chronyd**(8) service.

`udsAddr`
: - The Unix Domain Socket address used for inter-process communication with
**chronyd**(8) service.  
- Default parameter: "/var/run/chrony/chronyd.sock"

# EXAMPLES

Below are examples of JSON configuration files that users can create refer for
different scenarios.

1. Single ptp4l instance

    In this scenario, the system has only one `ptp4l` instance configured under
    a single time base. No `chrony` service is included. Users are expected
    to subscribe to the synchronization results from the `ptp4l` service. The
    JSON configuration includes only the `ptp4l` object within the time base.

    **JSON file content:**  

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

1. Single chrony instance

    In this scenario, the system has only one `chrony` instance configured under
    a single time base. No `ptp4l` service is included. Users are expected
    to subscribe to the synchronization results from the `chrony` service. The
    JSON configuration includes only the `chrony` object within the time base.

    **JSON file content:**  

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

1. Single ptp4l + Single chrony instance (default value)

    In this scenario, the system has one `ptp4l` instance and one `chrony`
    instance, both are grouped under a single time base. Users are
    expected to subscribe to the synchronization results from both `ptp4l` and
    `chrony`. Therefore, both services are configured within the same time base
    in the JSON configuration.
    If any fields in the `ptp4l` or `chrony` objects are not specified, default
    values will be assigned as follows:

    **JSON file content:**  

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

1. Single ptp4l + Single chrony instance (with assigned values)

    In this scenario, the system has one `ptp4l` instance and one `chrony`
    instance, both are grouped under a single time base. Users are expected to
    subscribe to the synchronization results from both `ptp4l` and `chrony`.
    Therefore, both services are configured within the same time base
    in the JSON configuration.

    **JSON file content:**  

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

1. Multiple ptp4l + Single chrony instances

    In this scenario, the system has multiple `ptp4l` instances and one `chrony`
    instance. Each `ptp4l` instance is configured under its own time base, while
    the `chrony` instance is grouped with one of the time bases. Users are
    expected to subscribe to the synchronization results from both `ptp4l`
    and `chrony`. Therefore, the JSON configuration includes multiple time bases,
    each with its own `ptp4l` configuration, and one of them also includes the
    `chrony` configuration.

    **JSON file content:**  

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

# FILES

/etc/clkmgr/proxy_cfg.json  
:   Contains example of time base configuration data for **clkmgr_proxy**(8).

# SEE ALSO

**clkmgr_proxy**(8),
**ptp4l**(8),
**chronyd**(8)
