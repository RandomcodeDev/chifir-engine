using Sharpmake;

namespace Krasnyy
{
	[Generate]
	public class FrameworkProject : BaseProject
	{
		public FrameworkProject()
		{
			Name = "Framework";

			AddTargets(Globals.GetTargets());
		}

		new public void ConfigureAll(Project.Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);

			conf.Output = Configuration.OutputType.Dll;
		}
	}
}
