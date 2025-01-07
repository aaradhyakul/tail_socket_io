gen_logs:
	g++ random_write.cpp -o rw && ./rw

clean:
	rm -f rw
