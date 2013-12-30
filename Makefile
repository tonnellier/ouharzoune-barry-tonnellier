OPTIONS = -Wall

all: mdazi service-notif client-notif

mdazi:
	gcc main.c -o mdazi $(OPTIONS)

service-notif:
	gcc service-notif.c -o service-notif $(OPTIONS)

client-notif:
	gcc client-notif.c -o client-notif $(OPTIONS)

clean:
	rm mdazi service-notif client-notif