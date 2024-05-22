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

		new public void ConfigureAll(Project.Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);

			conf.AddPublicDependency<FrameworkProject>(target);
		}
	}
}
