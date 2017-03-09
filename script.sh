
echo "The script starts now"

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile1"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile2"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile3"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile4"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile5"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile6"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile7"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile8"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile9"
done

for i in {1..5} ; do
  ./client -s 54.172.4.90 -c "cat testfile10"
done

echo "END"
