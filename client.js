const { createSocket } = require('dgram');

const VCTL_ADDR = '127.0.0.1'; 
const VCTL_PORT = '6666';

function volumectl_set_volume(udp_sock, num) {
    // Clamp @num to [0, 100]
    num = num < 0 ? 0 : num > 100 ? 100 : num;
    // Send a single byte representing @num
    udp_sock.send(Buffer.from([num]), VCTL_PORT, VCTL_ADDR, () => {
        udp_sock.close();
    });
}

const udp_sock = createSocket('udp4');
volumectl_set_volume(udp_sock, Number(process.argv[2]));
