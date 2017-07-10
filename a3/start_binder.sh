set -e
CUSTOM_TEST_FOLDER=CustomTestCode

BINDER_FOLDER=rpc

if  [ ! -f $BINDER_FOLDER/binder ]; then
    echo "Couldn't find the binder!"
    exit
fi

# Handles a race condition
:> ~/binder.log

# Startup binder. Output env vars to a global file
$BINDER_FOLDER/binder > ~/binder.log &


# Wait until file is done writing to
while [ `cat ~/binder.log | grep BINDER_PORT | wc -l` -lt 1 ]; do :; done
BINDER_ADDRESS=`cat ~/binder.log | head -2 | tail -1 | sed 's/BINDER_ADDRESS //'`
BINDER_PORT=`cat ~/binder.log | tail -n 1 | sed 's/BINDER_PORT //'`

# Put the export commands in a file, so a server sets them on start up
:>~/a3_env_vars
echo "export BINDER_ADDRESS=${BINDER_ADDRESS}" | tee -a ~/a3_env_vars
echo "export BINDER_PORT=${BINDER_PORT}" | tee -a ~/a3_env_vars

echo "Started binder successfully"


### SSH into other machines and start servers with this binders variables
HOSTS=("ubuntu1404-004.student.cs.uwaterloo.ca")
# "ubuntu1404-004.student.cs.uwaterloo.ca")
#"ubuntu1404-006.student.cs.uwaterloo.ca" "ubuntu1404-008.student.cs.uwaterloo.ca" "ubuntu1404-010.student.cs.uwaterloo.ca" "ubuntu1404-012.student.cs.uwaterloo.ca")

# Run on the server and put into the HOSTNAME.log
for server in "${HOSTS[@]}"
do
   #ssh "a9palmer@$server" 'ls'
   #ssh "a9palmer@$server" '. ~/CS454/a3/set_env_vars.sh'
   ssh "a9palmer@$server" 'ls'
done

#HOSTS=("ubuntu1204-002" "ubuntu1204-004" "ubuntu1204-006")
	# ssh into a single machine
	# run bash ~/CS454/a3/start_single_server.sh

echo "DONE"
