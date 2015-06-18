import sys
import codecs
import re



if __name__ == "__main__":

    p = re.compile("([0-9]\. )?(?P<word>.*);.*")
    words = set()
    for line in  codecs.open(sys.argv[1]):
        m = p.match(line)
        if m:
            clean = m.group('word')
            if len(clean) > 1:
                words.add(clean)
            
        
    with codecs.open("out.txt", 'w', 'utf-8-sig') as f:
        f.write("i\n");
        for clean in words:
            f.write(clean + "\n")

