All: twitterTrend_client twitterTrend_server
twitterTrend_client: twitterTrend_client.c
	gcc -o twitterTrend_client twitterTrend_client.c
twitterTrend_server: twitterTrend_server.c
	gcc -pthread -o twitterTrend_server twitterTrend_server.c
clean:
	rm *.result twitterTrend_client twitterTrend_server
