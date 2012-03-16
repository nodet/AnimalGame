cd ..\Tests

for %%f in (*.) do ..\Debug\AnimalGame.exe < %%f > %%f.test
for %%f in (*.) do c:\cygwin\bin\diff %%f.out %%f.test
