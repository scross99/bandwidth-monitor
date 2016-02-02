Bandwidth Monitor Kernel Module
-------------------------------

.. warning::
	This is a kernel module; if it crashes so will your computer.

This is a module to monitor all bandwidth going in/out your computer, identify
non-local traffic and applying bandwidth rate limiting as directed by user space
processes (two examples of which are included with the module).

By default, the limit (per second) will be set to 0 when this module is enabled,
meaning all traffic will be stopped, except for local traffic. It is necessary
to run one of the userspace programs (either the gui or the command line program)
which will control the module and set the limit to an appropriate amount, which
will depend on what limit (probably over a longer period of time) it is trying to
enforce.

Multiple userspace processes can control the module simultaneously, and the module
will always limit bandwidth usage to the smallest limit directed by the userspace
processes. When all userspace processes exit (or crash), the module will again stop
all network traffic.

Currently, the meaning of local traffic is hardcoded into the module. In future
it may be customisable outside of the module.

The module creates a number of 'files' at ``/sys/kernel/bandwidth/`` which can be
used to access the download bandwidth used, upload bandwidth used, the current
limit on the total bandwidth and the seconds from the perspective of the kernel
module (when the second value changes, the kernel module will update the values
and apply the new limit).

