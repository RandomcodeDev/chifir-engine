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
		}
	}
}
