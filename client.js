const { createSocket } = require('dgram');

const VCTL_ADDR = '127.0.0.1'; 
const VCTL_PORT = '6666';

function vctl_set_volume(udp_sock, num) {
    udp_sock.send(Buffer.from([num]), VCTL_PORT, VCTL_ADDR, () => {
        udp_sock.close();
    });
}

const udp_sock = createSocket('udp4');
vctl_set_volume(udp_sock, Number(process.argv[2]));
