all: monitor sensor

monitor:
	gcc monitor.c -o monitor

sensor:
	gcc sensor.c -o sensor

run: all
	@./sensor -s 1 -t 1 -f sensor-temperatura.txt -p pipe_nominal & \
	./sensor -s 2 -t 2 -f sensor-ph.txt -p pipe_nominal & \
	./sensor -s 2 -t 1 -f sensor-ph.txt -p pipe_nominal & \
	./sensor -s 1 -t 2 -f sensor-temperatura.txt -p pipe_nominal & \
	./monitor -b 100 -t temp.txt -h ph.txt -p pipe_nominal

runs1:
	./sensor -s 1 -t 2 -f sensor-temperatura.txt -p pipe_nominal

runs2:
	./sensor -s 2 -t 2 -f sensor-ph.txt -p pipe_nominal

runm:
	./monitor -b 100 -t temp.txt -h ph.txt -p pipe_nominal
 

clean:
	rm monitor sensor


