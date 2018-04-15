using Microsoft.Azure.Mobile.Server;
using Microsoft.OData.Edm;

namespace JKApplicationService.DataObjects
{
    public class TracksItem : EntityData
    {
        private float distance { get; set; }

        private Date travelTime { get; set; }

        private Date finishTime { get; set; }

        private int calBurned { get; set; }

        private string travellingPoints { get; set; }
    }
}