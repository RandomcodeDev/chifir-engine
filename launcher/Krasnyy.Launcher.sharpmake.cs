using Sharpmake;

namespace Krasnyy
{
	[Generate]
	public class LauncherProject : BaseProject
	{
		public LauncherProject()
		{
			Name = "Launcher";

			AddTargets(Globals.GetTargets());
		}

		public override void ConfigureAll(Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);

			conf.AddPublicDependency<FrameworkProject>(target);

			if (target.Optimization != Optimization.Debug && (target.Platform == Platform.win32 || target.Platform == Platform.win64))
			{
				conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
			}
		}
	}
}
