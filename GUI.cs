using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace DLLInjector
{
    public partial class MainForm : Form
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr OpenProcess(uint dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out IntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);

        public MainForm()
        {
            InitializeComponent();
        }

        private void btnSelectProcess_Click(object sender, EventArgs e)
        {
            // Find the Calculator process
            var calculatorProcess = Process.GetProcessesByName("calc").FirstOrDefault();
            if (calculatorProcess != null)
            {
                txtProcessName.Text = calculatorProcess.ProcessName;
                txtProcessId.Text = calculatorProcess.Id.ToString();
            }
            else
            {
                MessageBox.Show("Calculator process not found. Please start the Calculator application.");
            }
        }

        private void btnInject_Click(object sender, EventArgs e)
        {
            if (int.TryParse(txtProcessId.Text, out int processId))
            {
                string dllPath = @"path\to\your\dll.dll"; // Replace with the path to your DLL file
                InjectDLL(processId, dllPath);
            }
            else
            {
                MessageBox.Show("Invalid process ID.");
            }
        }

        private void InjectDLL(int processId, string dllPath)
        {
            if (!File.Exists(dllPath))
            {
                MessageBox.Show("DLL not found.");
                return;
            }

            IntPtr processHandle = OpenProcess(0x2 | 0x8 | 0x10 | 0x20 | 0x400, false, processId);
            if (processHandle == IntPtr.Zero)
            {
                MessageBox.Show("Failed to open process.");
                return;
            }

            IntPtr loadLibraryAddress = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
            if (loadLibraryAddress == IntPtr.Zero)
            {
                MessageBox.Show("Failed to find LoadLibraryA function.");
                CloseHandle(processHandle);
                return;
           
            byte[] dllPathBytes = System.Text.Encoding.ASCII.GetBytes(dllPath);
            IntPtr dllPathAddress = VirtualAllocEx(processHandle, IntPtr.Zero, (uint)dllPathBytes.Length, 0x1000 | 0x2000, 0x4);
            if (dllPathAddress == IntPtr.Zero)
            {
                MessageBox.Show("Failed to allocate memory for DLL path.");
                CloseHandle(processHandle);
                return;
            }

            if (!WriteProcessMemory(processHandle, dllPathAddress, dllPathBytes, (uint)dllPathBytes.Length, out _))
            {
                MessageBox.Show("Failed to write DLL path to process memory.");
                CloseHandle(processHandle);
                return;
            }

            IntPtr threadHandle = CreateRemoteThread(processHandle, IntPtr.Zero, 0, loadLibraryAddress, dllPathAddress, 0, IntPtr.Zero);
            if (threadHandle == IntPtr.Zero)
            {
                MessageBox.Show("Failed to create remote thread.");
                CloseHandle(processHandle);
                return;
            }

            MessageBox.Show("DLL injection successful.");
            CloseHandle(processHandle);
        }
    }
}
