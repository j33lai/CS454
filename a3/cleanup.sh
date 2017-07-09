#  Don't put the word 'binder' in the name of this script, otherwise it will kill itself
if [ `ps axef | grep \`whoami\` | grep -E "binder|server" | grep -v grep | wc -l` -ne 0 ]
then
    echo "Killing previous instances of binder and server"
    cmds=$(ps axef | grep `whoami` | grep -E "binder|server" | grep -v grep | awk '{print "kill " $1 ";"}')
    echo "${cmds}"
    eval ${cmds}
else
    echo "No previous instances of binder or server"
fi
