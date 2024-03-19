# JCLKLIB codeflow

Jclklib is a 2-part implementation for C/C++ application to obtain
ptp4l events via pub/sub method using a library api call.

It provides a library jcklib.so (aka client-runtime library) and a daemon
jclklib_proxy.

In short we have :
           FRONT-END              MIDDLE-PIPE               BACK-END
c/c++ app <---------> jclklib.so <----------> jclklib_proxy<--------> ptp4l
        (library call)           (using msq)      |         (via UDS)
                                                  |
                                              libptpmgmt.so

* **c++ app** - Customer application that will be linking to jclklib.so library. Header file and sample test cpp file will be provided.

* **jcklib.so** - A dynamic library that provides a set of APIs to customer application : connect/disconnect, subscribe to ptp4l daemon. This library is written in C++. It will have a permissive licence. It communicates with jclkib_proxy using message queues.

* **jclklib_proxy** - A daemon that is using libptpmgmt api to establish connection and subscribe towards ptp4l events. The communication is establish using ptp4l UDS (/var/run/ptp4l as we now assumed it is always installed in the same local machine as the ptp4l)

## Connect message flow (simplified code walkthrough)

