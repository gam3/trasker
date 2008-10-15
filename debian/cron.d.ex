#
# Regular cron jobs for the tasker package
#
0 4	* * *	root	[ -x /usr/bin/tasker_maintenance ] && /usr/bin/tasker_maintenance
