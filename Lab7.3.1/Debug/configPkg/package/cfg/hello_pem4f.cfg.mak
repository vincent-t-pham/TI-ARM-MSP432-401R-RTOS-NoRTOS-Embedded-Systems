# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f C:\Users\VINCEN~1\workspace_v10\Lab7.3/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\VINCEN~1\workspace_v10\Lab7.3/src/makefile.libs clean

