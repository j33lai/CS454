#####
# Needs to execute the script in the current shell without forking a sub shell.
# Usage:
# . ./setup.bash
##

set -e
CURR_MACHINE=`hostname`
CURR_MACHINE_LOG_FILE=~/"$CURR_MACHINE"_server.log
PARTIAL_TEST_FOLDER=PartialTestCode

# Just for partial tests
if [ ! -f $PARTIAL_TEST_FOLDER/server ] || [ ! -f $PARTIAL_TEST_FOLDER/client ] ; then
    echo "Couldn't find the partial test server or client!"
    exit
fi

# Init the log file
:>$CURR_MACHINE_LOG_FILE

# Set the env variables for this server. Waits for the file to be written to. 
# todo: add a timeout here
if [ ! -f ~/a3_env_vars ] ; then
    echo "Couldn't find the env vars file!"
    exit
fi

while [ `cat ~/a3_env_vars | grep BINDER_PORT | wc -l` -lt 1 ]; do :; done
source ~/a3_env_vars

# Write to log file
echo "Set the binder address to: $BINDER_ADDRESS" >> $CURR_MACHINE_LOG_FILE
echo "Set the binder port to: $BINDER_PORT" >> $CURR_MACHINE_LOG_FILE
echo "Starting up a server on this machine" >> $CURR_MACHINE_LOG_FILE


###### Actually startup the partial server
$PARTIAL_TEST_FOLDER/server &
echo "Server started successfully" >> $CURR_MACHINE_LOG_FILE
