from __future__ import with_statement
from fabric.api import env, run, parallel, hide, put

env.hosts = [
    "slice400.pcvm3-1.geni.case.edu",
    "slice400.pcvm3-1.instageni.metrodatacenter.com",
    "slice400.pcvm3-2.instageni.illinois.edu",
    "slice400.pcvm5-7.lan.sdn.uky.edu",
    "slice400.pcvm3-1.instageni.lsu.edu",
    "slice400.pcvm2-2.instageni.maxgigapop.net",
    "slice400.pcvm1-1.instageni.iu.edu",
    "slice400.pcvm3-4.instageni.rnet.missouri.edu",
    "slice400.pcvm2-1.instageni.nysernet.org",
    "slice400.pcvm3-11.genirack.nyu.edu",
    "slice400.pcvm5-2.instageni.cs.princeton.edu",
    "slice400.pcvm1-6.instageni.sox.net",
    "slice400.pcvm3-1.instageni.stanford.edu",
    "slice400.pcvm2-1.instageni.idre.ucla.edu",
    "slice400.pcvm4-1.utahddc.geniracks.net",
    "slice400.pcvm1-1.instageni.wisc.edu",
]

env.key_filename = "./id_rsa"
env.use_ssh_config = True
env.ssh_config_path = './ssh-config'


@parallel(pool_size=5)
def configure():
    with hide('stdout', 'stderr'):
        run('apt-get update')
        run('apt-get -y upgrade')
        run('apt-get -y install libprotobuf8 sqlite3')
        put('mac_chord', 'mac_chord')
        run('chmod +x mac_chord')


@parallel(pool_size=5)
def dependencies():
    with hide('stdout', 'stderr'):
        run('apt-get update')
        run('apt-get -y upgrade')
        run('apt-get -y install libprotobuf8 sqlite3')


@parallel(pool_size=5)
def program():
    put('mac_chord', 'mac_chord')
    run('chmod +x mac_chord')


def ips():
    run("echo `ifconfig eth0 | awk '/(cast)/ {print $2 }' | cut -d ':' -f2 | head -1`")
