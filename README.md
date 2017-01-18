### Safe Execution of Untrusted x86 Machine Code Using Hardware-Assisted Virtualization

<sup>**Yi-Fan Zhang**</sup><br/>
<sup>January 2017</sup>
#### Abstract
The ability to confine user-space software is critical for protecting users from malicious or dubious application behavior. With that in mind, we designed and implemented Hvexec, a virtualization layer that is interposed between applications and the kernel. Clients of Hvexec are given an API which they can use to monitor, filter or transform system calls from any application. To achieve this without requiring re-compiling or editing the application binary, we implemented Hvexec using an atypical application of hardware-assisted virtualization where the restricted software executes in the context of a hardware virtual machine. In this [paper](https://github.com/yifanz/masters-proj/blob/master/doc/report.pdf), we describe the benefits of using virtualization, the design and lessons learnt from our implementation and benchmarks to weight the benefits against the overhead.
