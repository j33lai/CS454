# Set the env variables for this server. Waits for the file to be written to. 
# todo: add a timeout here
if [ ! -f ~/a3_env_vars ] ; then
    echo "Couldn't find the env vars file!"
    exit
fi

while [ `cat ~/a3_env_vars | grep BINDER_PORT | wc -l` -lt 1 ]; do :; done
source ~/a3_env_vars
