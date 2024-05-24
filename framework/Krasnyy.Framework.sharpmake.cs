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

		public override void ConfigureAll(Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);

			conf.Defines.Add("KR_FRAMEWORK=1");

			conf.Output = Configuration.OutputType.Dll;

			conf.AddPublicDependency<MimallocProject>(target);
		}
	}
}
