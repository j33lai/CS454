PARTIAL_TEST_FOLDER=PartialTestCode
CUSTOM_TEST_FOLDER=CustomTestCode

BINDER_FOLDER=rpc

if [ ! -f $PARTIAL_TEST_FOLDER/server ] || [ ! -f $PARTIAL_TEST_FOLDER/client ] ; then
    echo "Couldn't find the partial test server or client!"
    exit
fi

if  [ ! -f $BINDER_FOLDER/binder ]; then
    echo "Couldn't find the binder!"
    exit
fi

#echo "Found the partial test cases and the binder"

#  Get rid of race condition where binder_out file already has stuff in it and causes startup to continue
#  before server address has been output.
:> binder_out

#  Start the binder and pipe the output to a file to a file so we can automatically set the environment variables
$BINDER_FOLDER/binder > binder_out &

# Wait until file is done writing to
while [ `cat binder_out | grep BINDER_PORT | wc -l` -lt 1 ]; do :; done
BINDER_ADDRESS=`cat binder_out | head -2 | tail -1 | sed 's/BINDER_ADDRESS //'`
BINDER_PORT=`cat binder_out | tail -n 1 | sed 's/BINDER_PORT //'`

export BINDER_ADDRESS=${BINDER_ADDRESS}
export BINDER_PORT=${BINDER_PORT}

echo "Binder started"
echo "Set the binder address to: $BINDER_ADDRESS"
echo "Set the binder port to: $BINDER_PORT"

#HOSTS=("ubuntu1204-002" "ubuntu1204-004" "ubuntu1204-006")

# Run servers
for i in {1..5}
do
    #./custom_server &
    $PARTIAL_TEST_FOLDER/server &
    sleep 1
done

echo "Servers started"

#  Wait a couple seconds for the servers to register
echo "Launching clients..."

# Run clients
for i in {1..5}
do
    #./custom_client
    # $PARTIAL_TEST_FOLDER/client &
    echo ""
done

sleep 1

#  Run the client provided in the assignemnt code
echo "Launching assignment verion of client..."
echo "y" | $PARTIAL_TEST_FOLDER/client

echo "\n"
