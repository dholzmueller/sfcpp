for f in **/*.hpp; do
  cat ./header.txt $f > $f.new
  mv $f.new $f
done

for f in **/*.cpp; do
  cat ./header.txt $f > $f.new
  mv $f.new $f
done


