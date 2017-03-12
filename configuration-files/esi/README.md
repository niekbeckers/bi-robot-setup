# README ESI

This folder contains the ESI (EtherCAT Slave Information) files for the Finger Manipulator:

+ gsv8esi_v18 [[link]](https://www.me-systeme.de/shop/de/elektronik/gsv-8/gsv-8ds/gsv-8ds4?action_ms=1)
+ iPOS-CAT-XML2 [[link]](http://www.technosoftmotion.com/en/intelligent-drives-and-motors/ipos-line/ipos4808/ipos4808-bx-cat#downloads)

## TwinCAT

To use these ESI files (the files with extension .xml) in TwinCAT, add them to the following path:

```
C:\TwinCAT\3.1\Config\Io\EtherCAT
```

Then, in TwinCAT, once you have added an EtherCAT master, you can add the slaves (make sure to use the correct order!) by `right-click on EtherCAT master` &rarr; `Add new item` &rarr; `Select correct slave from list`.
