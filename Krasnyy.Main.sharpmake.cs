using System.Collections;

using Sharpmake;

[module: Sharpmake.Include("Krasnyy.*.sharpmake.cs")]
[module: Sharpmake.Include("framework/Krasnyy.Framework.sharpmake.cs")]
[module: Sharpmake.Include("launcher/Krasnyy.Launcher.sharpmake.cs")]

namespace Krasnyy
{
	public static class Globals
	{
		public static string RootDirectory;

		public static Optimization Optimizations = Optimization.Debug | Optimization.Release | Optimization.Retail;

		public static Target WindowsTarget = new Target(Platform.win32 | Platform.win64, DevEnv.vs2022, Optimizations);
		public static Target LinuxTarget = new Target(Platform.linux, DevEnv.make, Optimizations);

		public static Target[] GetTargets()
		{
			return new Target[] {WindowsTarget, LinuxTarget};
		}
	}
	
	public static class Main
	{
		[Sharpmake.Main]
		public static void SharpmakeMain(Sharpmake.Arguments args)
		{
			KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2022, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.Latest);
	
			Globals.RootDirectory = Util.GetCurrentSharpmakeFileInfo().DirectoryName;

			args.Generate<Krasnyy.MainSolution>();
		}
	}
}

