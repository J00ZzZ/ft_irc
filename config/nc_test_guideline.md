```bash
nc 127.0.0.1 667
ctrl+v + (ctrl+m) to send carriage return

(echo -n "com"; sleep 1; echo -n "man"; sleep 1; echo -n "d\n") | nc -C 127.0.0.1 6667
sends partial data

```
### script to sim partial tcp packets
``` bash
{
  printf "N"
  sleep 0.5
  printf "ICK olek\r\n"
  sleep 1
  printf "US"
  sleep 0.5
  printf "ER testuser 0 * :olek\r\n"
  sleep 1
  printf "PA"
  sleep 0.5
  printf "SS bajo\r\n"
  sleep 1
  printf "JO"
  sleep 0.5
  printf "IN #ch1\r\n"
} | nc 127.0.0.1 6667
```
https://stackoverflow.com/questions/10250666/socket-programming-issue-with-recv-receiving-partial-messages