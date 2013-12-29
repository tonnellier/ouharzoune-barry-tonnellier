OPTIONS = -Wall

all: mdazi service-notif

mdazi:
	gcc main.c -o mdazi $(OPTIONS)

service-notif:
	gcc service-notif.c -o service-notif $(OPTIONS)

clean:
	rm mdazi service-notif